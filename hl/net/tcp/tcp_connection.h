#ifndef HL_TCP_TCP_CONNECTION_H
#define HL_TCP_TCP_CONNECTION_H
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_buffer.h"
#include"/home/hl/hl-tinyrpc/hl/net/io_thread.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_protocol.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_coder.h"
#include<memory>
#include<map>
#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_dispatcher.h"
namespace hl
{

enum TcpState{
    NotConnected=1,
    Connected=2,
    HalfClosing=3,
    Closed=4,
};
enum TcpConnectionType{
    TcpConnectionByServer=1,
    TcpConnectionByClient=2,
};
class TcpConnection{

public:

/// @brief 使用枚举结构表示连接状态


typedef std::shared_ptr<TcpConnection>s_ptr;

TcpConnection(EventLoop*eventloop,int fd,int buffer_size,NetAddr::s_ptr peer_addr,NetAddr::s_ptr local_addr,TcpConnectionType type=TcpConnectionByServer);

~TcpConnection();


void onRead();

void excute();

void onWrite();

void setState(const TcpState state);

TcpState getState();

void clear();

//服务器主动关闭连接
void shutDown();

//启动监听可写事件
void listenWrite();

void listenRead();

void setConnectionType(TcpConnectionType type);

void pushSendMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)> done);

void pushReadMessage(const std::string&req_id,std::function<void(AbstractProtocol::s_ptr)> done);

NetAddr::s_ptr getLocalAddr();

NetAddr::s_ptr getPeerAddr();


private:

EventLoop*m_event_loop{NULL};//持有该连接的IO线程

NetAddr::s_ptr m_peer_addr;

NetAddr::s_ptr m_local_addr;

TcpBuffer::s_ptr m_in_buffer;//输入缓冲区

TcpBuffer::s_ptr m_out_buffer;//输出缓冲区



FdEvent*m_fd_event{NULL};

TcpState m_state;//当前状态

int m_fd{0};

TcpConnectionType m_connection_type{TcpConnectionByServer};

//pair<AbstractProtocol::s_ptr,std::function<void(AbstractProtocol::s_ptr)>
std::vector<std::pair<AbstractProtocol::s_ptr,std::function<void(AbstractProtocol::s_ptr)>>>m_write_dones;//写入的回调函数

std::map<std::string ,std::function<void(AbstractProtocol::s_ptr)>>m_read_dones;//写入的回调函数



AbstractCoder*m_coder{NULL};
};
} // namespace hl


#endif