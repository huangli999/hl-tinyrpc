#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_channel.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/tinypb_protocol.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/msg_id_util.h"
#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_controller.h"
#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_closure.h"
#include<google/protobuf/service.h>
#include<google/protobuf/descriptor.h>
#include<google/protobuf/message.h>
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_client.h"
#include"/home/hl/hl-tinyrpc/hl/common/error_code.h"
#include"/home/hl/hl-tinyrpc/hl/net/timer_event.h"
namespace hl
{

    RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr):m_peer_addr(peer_addr){
        m_client=std::make_shared<TcpClient>(m_peer_addr);
    }
        
    RpcChannel::~RpcChannel(){
            INFOLOG("rpc close",NULL);
    }

    void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
        google::protobuf::RpcController* controller, const google::protobuf::Message* request,
        google::protobuf::Message* response, google::protobuf::Closure* done){
            std::shared_ptr<hl::TinyPBProtocol>req_protocol=std::make_shared<hl::TinyPBProtocol>();

            RpcConroller*my_controller=dynamic_cast<RpcConroller*>(controller);
            if(my_controller==NULL){
                ERRORLOG("failed callmethod,RpcController covert error",NULL);
                return;
            }
            //是否初始化成功
            if(!m_is_init){
                std::string err_info="RpcChannle init error";
                my_controller->SetError(ERROR_RPC_CHANNEL_INIT,err_info);
                ERRORLOG("%s|%s,origin request[%s]",req_protocol->m_msg_id.c_str(),err_info.c_str(),request->ShortDebugString().c_str());
                return;
            }


            if(my_controller->GetMsgId().empty()){
                req_protocol->m_msg_id=MsgIDUtil::GenMsgID();
                my_controller->SetMsgId(req_protocol->m_msg_id);
            }else{

                req_protocol->m_msg_id=my_controller->GetMsgId();
            }

            req_protocol->m_method_name=method->full_name();
            INFOLOG("%s|call method name [%s]",req_protocol->m_msg_id.c_str(),req_protocol->m_method_name.c_str());
            //requst 的序列化
            if(!request->SerializePartialToString(&(req_protocol->m_pb_data))){
                std::string err_info="failed to serialize";
                my_controller->SetError(ERROR_FAILED_SERIALIZE,err_info);
                ERRORLOG("%s|%s,origin request[%s]",req_protocol->m_msg_id.c_str(),err_info.c_str(),request->ShortDebugString().c_str());
                return;
            }

            s_ptr channel=shared_from_this();

            m_timer_event=std::make_shared<TimerEvent>(my_controller->GetTimeout(),false,
            [my_controller,channel]()mutable{
                my_controller->StartCancel();
                my_controller->SetError(ERROR_RPC_CALL_TIMEOUT,"rpc call time out"+std::to_string(my_controller->GetTimeout()));
                
                if(channel->GetClosure()){
                    channel->GetClosure()->Run();
                }
                channel.reset();
            }
            );
            m_client->addTimerEvent(m_timer_event);

            m_client->connect([req_protocol,channel]()mutable{

                RpcConroller*my_controller=dynamic_cast<RpcConroller*>(channel->GetController());

                if(channel->getTcpClient()->getConnectErrorCode()!=0){
                    ERRORLOG("%s|connect error,error code[%d],error info[%s],peer_addr[%s]",req_protocol->m_msg_id.c_str(),
                my_controller->GetErrorCode(),my_controller->GetErrorInfo().c_str(),channel->getTcpClient()->getPeerAddr()->toString().c_str());
                    my_controller->SetError(channel->getTcpClient()->getConnectErrorCode(),channel->getTcpClient()->getConnectErrorInfo());
                }
                channel->getTcpClient()->writeMessage(req_protocol,[req_protocol,channel,my_controller](AbstractProtocol::s_ptr)mutable{
                    INFOLOG("%s|send request success,method_name[%s]",req_protocol->m_msg_id.c_str(),
                    req_protocol->m_method_name.c_str());
                    channel->getTcpClient()->readMessage(req_protocol->m_msg_id,[channel,my_controller](AbstractProtocol::s_ptr msg)mutable{
                        std::shared_ptr<hl::TinyPBProtocol>rsp_protocol=std::dynamic_pointer_cast<hl::TinyPBProtocol>(msg);
                        INFOLOG("%s|get responce success,method_name[%s],peer_addr[%s],local_addr[%s]",rsp_protocol->m_msg_id.c_str(),rsp_protocol->m_method_name.c_str(),
                        channel->getTcpClient()->getPeerAddr()->toString().c_str(),channel->getTcpClient()->getLocalAddr()->toString().c_str());
                    
                        //当成功读取到回包取消定时任务
                        channel->getTimerEvent()->setCancle(true);

                        if(!(channel->GetReSponse()->ParseFromString(rsp_protocol->m_pb_data))){
                            ERRORLOG("%s|serialize error",rsp_protocol->m_msg_id.c_str());
                            my_controller->SetError(ERROR_FAILED_SERIALIZE,"serialize error");
                            return;
                        }

                        if(rsp_protocol->m_err_code!=0){
                            ERRORLOG("%s|call rpc failed",rsp_protocol->m_msg_id.c_str());
                            my_controller->SetError(rsp_protocol->m_err_code,rsp_protocol->m_err_info);
                            return;
                        }
                        INFOLOG("%s|rpc success,method_name[%s],peer_addr[%s],local_addr[%s]",rsp_protocol->m_msg_id.c_str(),rsp_protocol->m_method_name.c_str(),
                        channel->getTcpClient()->getPeerAddr()->toString().c_str(),channel->getTcpClient()->getLocalAddr()->toString().c_str());
                        

                        
                        if(!my_controller->IsCanceled()&&channel->GetClosure()){
                            channel->GetClosure()->Run();
                        }
                        channel.reset();
                    
                    });

                });
            });
        }



        void RpcChannel::init(controller_s_ptr controller,message_s_ptr req,message_s_ptr res,closure_s_ptr done){

            if(m_is_init){
                return;
            }
            m_controller=controller;
            m_request=req;
            m_response=res;
            m_closure=done;
            m_is_init=true;
        }


        
        google::protobuf::RpcController*RpcChannel::GetController(){
            return m_controller.get();
        }

        google::protobuf::Message*RpcChannel::GetRequest(){
            return m_request.get();
        }

        google::protobuf::Message*RpcChannel::GetReSponse(){
            return m_response.get();
        }

        google::protobuf::Closure*RpcChannel::GetClosure(){
            return m_closure.get();
        }

        TcpClient*RpcChannel::getTcpClient(){
            return m_client.get();
        }

        TimerEvent::s_ptr RpcChannel::getTimerEvent(){
            return m_timer_event;
        }
} // namespace hl
