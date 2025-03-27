#ifndef HL_TCP_TCP_CLIENT_H
#define HL_TCP_TCP_CLIENT_H
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_connection.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_protocol.h"
#include<memory>
#include"/home/hl/hl-tinyrpc/hl/net/timer_event.h"
namespace hl
{
class TcpClient{
public:
typedef std::shared_ptr<TcpClient>s_ptr;

TcpClient(NetAddr::s_ptr peer_addr);

~TcpClient();

//异步的进行connect
//如果connect成功，done会被执行
void connect(std::function<void()>done);

//异步的发送Message
//如果发送成功，会调用done函数
void writeMessage(AbstractProtocol::s_ptr request,std::function<void(AbstractProtocol::s_ptr)>done);

//异步的读取Message
//如果读取成功，会调用done函数
void readMessage(const std::string &req_id,std::function<void(AbstractProtocol::s_ptr)>done);

void stop();//结束loop悬环

NetAddr::s_ptr getPeerAddr();//获得对端地址

NetAddr::s_ptr getLocalAddr();//获得当前地址

void initLocalAddr();//初始化当前地址

int getConnectErrorCode();

std::string getConnectErrorInfo();

void addTimerEvent(TimerEvent::s_ptr timeevent);


private:
NetAddr::s_ptr m_peer_addr;
NetAddr::s_ptr m_local_addr;

EventLoop*m_event_loop{NULL};

int m_fd{-1};
FdEvent*m_fd_event{NULL};


TcpConnection::s_ptr m_connection;

int m_connect_errcode{0};

std::string m_connect_error_info;


};
} // namespace hl


#endif