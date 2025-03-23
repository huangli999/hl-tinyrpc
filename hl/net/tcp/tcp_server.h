#ifndef HL_NET_TCP_TCP_SERVER_H
#define HL_NET_TCP_TCP_SERVER_H
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_acceptor.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include"/home/hl/hl-tinyrpc/hl/net/io_thread_group.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_connection.h"
#include<set>

namespace hl
{
class TcpServer{

public:
TcpServer(NetAddr::s_ptr local_addr);

~TcpServer();

void start();//开启loop悬环

private:

void init();//创建监听事件

void onAccept();//当有新连接之后执行

private:
TcpAcceptor::s_ptr m_acceptor;

NetAddr::s_ptr m_local_addr;//本地监听地址

EventLoop*m_main_event_loop{NULL};//mainReactor

IOThreadGroup* m_io_thread_group{NULL};//subEeactor

FdEvent*m_listen_fd_event;

int m_client_counts{0};//客户端吧连接数量

std::set<TcpConnection::s_ptr>m_client;
};
} // namespace hl




#endif