#ifndef HL_NET_TCP_TCP_ACCEPTOR_H
#define HL_NET_TCP_TCP_ACCEPTOR_H
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include<memory>
namespace hl
{
    
class TcpAcceptor{
public:

typedef std::shared_ptr<TcpAcceptor>s_ptr;

TcpAcceptor(NetAddr::s_ptr local_addr);

~TcpAcceptor();

int accept();

int getListenFd();

private:
//服务端监听的地址
NetAddr::s_ptr m_local_addr;

int m_family{-1};//协议

int m_listenfd{-1};//监听套接字



};
} // namespace hl



#endif