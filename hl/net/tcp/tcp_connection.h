#ifndef HL_TCP_TCP_CONNECTION_H
#define HL_TCP_TCP_CONNECTION_H
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_buffer.h"
#include"/home/hl/hl-tinyrpc/hl/net/io_thread.h"
#include<memory>
namespace hl
{

enum TcpState{
    NotConnected=1,
    Connected=2,
    HalfClosing=3,
    Closed=4,
};

class TcpConnection{

public:

/// @brief 使用枚举结构表示连接状态


typedef std::shared_ptr<TcpConnection>s_ptr;

TcpConnection(IOThread*io_thread,int fd,int buffer_size,NetAddr::s_ptr peer_addr);

~TcpConnection();


void onRead();

void excute();

void onWrite();

void setState(const TcpState state);

TcpState getState();

void clear();

//服务器主动关闭连接
void shutDown();

private:

IOThread*m_io_thread{NULL};//持有该连接的IO线程

NetAddr::s_ptr m_local_addr;

NetAddr::s_ptr m_peer_addr;

TcpBuffer::s_ptr m_in_buffer;//输入缓冲区
TcpBuffer::s_ptr m_out_buffer;//输出缓冲区



FdEvent*m_fd_event{NULL};

TcpState m_state;//当前状态

int m_fd{0};
};
} // namespace hl


#endif