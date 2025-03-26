#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_channel.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/tinypb_protocol.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/msg_id_util.h"
#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_controller.h"
#include<google/protobuf/service.h>
#include<google/protobuf/descriptor.h>
#include<google/protobuf/message.h>
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_client.h"
#include"/home/hl/hl-tinyrpc/hl/common/error_code.h"
namespace hl
{

    RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr):m_peer_addr(peer_addr){

    }
        
    RpcChannel::~RpcChannel(){

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

            if(my_controller->GetMsgId().empty()){
                req_protocol->m_msg_id=MsgIDUtil::GenMsgID();
                my_controller->SetMsgId(req_protocol->m_msg_id);
            }else{

                req_protocol->m_msg_id=my_controller->GetMsgId();
            }

            req_protocol->m_method_name=method->full_name();
            INFOLOG("%s|call method name [%s]",req_protocol->m_msg_id.c_str(),req_protocol->m_method_name.c_str());
            //requst 的序列化
            if(request->SerializePartialToString(&(req_protocol->m_pb_data))){
                std::string err_info="failed to serialize";
                my_controller->SetError(ERROR_FAILED_SERIALIZE,err_info);
                ERRORLOG("%s|%s,origin request[%s]",req_protocol->m_msg_id.c_str(),err_info.c_str(),request->ShortDebugString().c_str());
                return;
            }

            TcpClient client(m_peer_addr);

            client.connect([&client,req_protocol,done](){
                client.writeMessage(req_protocol,[&client,req_protocol,done](AbstractProtocol::s_ptr){
                    INFOLOG("%s|send request success,method_name[%s]",req_protocol->m_msg_id.c_str(),
                    req_protocol->m_method_name.c_str());
                    client.readMessage(req_protocol->m_msg_id,[done](AbstractProtocol::s_ptr msg){
                        std::shared_ptr<hl::TinyPBProtocol>rsp_protocol=std::dynamic_pointer_cast<hl::TinyPBProtocol>(msg);
                        INFOLOG("%s|get responce success,method_name[%s]",rsp_protocol->m_msg_id.c_str(),rsp_protocol->m_method_name.c_str());
                        
                        if(done){
                            done->Run();
                        }
                    
                    });

                });
            });
        }
} // namespace hl
