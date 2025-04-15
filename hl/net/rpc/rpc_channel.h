#ifndef HL_NET_RPC_RPC_CHANNEL_H
#define HL_NET_RPC_RPC_CHANNEL_H
#include <google/protobuf/service.h>
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_client.h"
#include<memory>
#include"/home/hl/hl-tinyrpc/hl/net/timer_event.h"
namespace hl
{
    #define NEWMESSAGE(type,var_name) \
    std::shared_ptr<type>var_name=std::make_shared<type>(); \

    #define NEWRPCCHANNEL(var_name,addr) \
    std::shared_ptr<hl::RpcChannel>var_name=std::make_shared<hl::RpcChannel>(std::make_shared<hl::IPNetAddr>(addr));\

    #define NEWRPCCONTROLLER(var_name) \
    std::shared_ptr<hl::RpcConroller>var_name=std::make_shared<hl::RpcConroller>();\

    #define CALLRPC(addr,stub_name,controller,request,response,closure,method_name) \
    {\
    NEWRPCCHANNEL(channel,addr);\
    channel->init(controller,request,response,closure);\
    stub_name(channel.get()).method_name(controller.get(),request.get(),response.get(),closure.get());\
    }\

    class RpcChannel: public google::protobuf::RpcChannel,public std::enable_shared_from_this<RpcChannel>{
        public:
        typedef std::shared_ptr<RpcChannel>s_ptr;
        typedef std::shared_ptr<google::protobuf::RpcController>controller_s_ptr;
        typedef std::shared_ptr<google::protobuf::Message>message_s_ptr;
        typedef std::shared_ptr<google::protobuf::Closure>closure_s_ptr;

        public:

        RpcChannel(NetAddr::s_ptr peer_addr);

        ~RpcChannel();

        void init(controller_s_ptr controller,message_s_ptr req,message_s_ptr res,closure_s_ptr done);

        void CallMethod(const google::protobuf::MethodDescriptor* method,
            google::protobuf::RpcController* controller, const google::protobuf::Message* request,
            google::protobuf::Message* response, google::protobuf::Closure* done);

        TimerEvent::s_ptr getTimerEvent();

        google::protobuf::RpcController*GetController();

        google::protobuf::Message*GetRequest();

        google::protobuf::Message*GetReSponse();

        google::protobuf::Closure*GetClosure();

        TcpClient*getTcpClient();
        
        private:
        NetAddr::s_ptr m_peer_addr{nullptr};
        NetAddr::s_ptr m_local_addr{nullptr};
        controller_s_ptr m_controller{nullptr};
        message_s_ptr m_request{nullptr};
        message_s_ptr m_response{nullptr};
        closure_s_ptr m_closure{nullptr};

        bool m_is_init{false};

        TcpClient::s_ptr m_client{nullptr};

        TimerEvent::s_ptr m_timer_event{nullptr};
    };
} // namespace hl



#endif