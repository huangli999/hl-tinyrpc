#ifndef HL_NET_RPC_DISPATCHER_H
#define HL_NET_RPC_DISPATCHER_H
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_protocol.h"
#include<map>
#include<memory>
#include<google/protobuf/service.h>
#include"/home/hl/hl-tinyrpc/hl/net/coder/tinypb_protocol.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_connection.h"
namespace hl
{
    class TcpConnection;
   class RpcDispatcher{

    public:
        static RpcDispatcher*GetRpcDispatcher();
    public:
    typedef std::shared_ptr<google::protobuf::Service> service_s_ptr;//重命名

    void dispatch(AbstractProtocol::s_ptr request,AbstractProtocol::s_ptr response,TcpConnection*connection);

    void registerService(service_s_ptr service);//注册service方法

    void setTinyPBError(std::shared_ptr<TinyPBProtocol>msg,int32_t err_code,const std::string err_info);
    private:
        bool parseServiceFullName(const std::string&full_name,std::string&service_name,std::string&method_name);
    
    private:
    std::map<std::string,std::shared_ptr<google::protobuf::Service>>m_service_map;
   };
} // namespace hl


#endif