#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_client.h"
#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include<sys/socket.h>
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event_group.h"
#include<unistd.h>
#include<string.h>
namespace hl
{
    TcpClient::TcpClient(NetAddr::s_ptr peer_addr):m_peer_addr(peer_addr){
        m_event_loop=EventLoop::GetCurrentEventLoop();
        m_fd=socket(peer_addr->getFamily(),SOCK_STREAM,0);
        if(m_fd<0){
            ERRORLOG("TcpClient::TcpClient()error",NULL);
            return;
        }
        m_fd_event=FdEventGroup::GetFdEventGroup()->getFdEvent(m_fd);
        m_fd_event->setNonBlock();
        m_connection=std::make_shared<TcpConnection>(m_event_loop,m_fd,128,peer_addr);
        m_connection->setConnectionType(TcpConnectionByClient);
    }

    TcpClient::~TcpClient(){
        if(m_fd>0){
            close(m_fd);
        }
    }

    //异步的进行connect
    //如果connect成功，done会被执行
    void  TcpClient::connect(std::function<void()>done){
        int rt=::connect(m_fd,m_peer_addr->getSockAddr(),m_peer_addr->getSockLen());
        if(rt==0){
            DEBUGLOG("connect[%s]success",m_peer_addr->toString().c_str());
            if(done){
                done();
            }
        }else if(rt==-1){
            if(errno==EINPROGRESS){
                m_fd_event->listen(FdEvent::OUT_EVENT,[this,done](){
                    int error=0;
                    socklen_t error_len=sizeof(error);
                    getsockopt(m_fd,SOL_SOCKET,SO_ERROR,&error,&error_len);
                    if(error==0){
                        DEBUGLOG("connect[%s]success",m_peer_addr->toString().c_str());
                        if(done){
                            done();
                        }
                    }else{
                        ERRORLOG("connect error,errno=%d,error=%s",errno,strerror(errno));
                    }
                    //连接完去掉可写事件监听
                    m_fd_event->cancle(FdEvent::OUT_EVENT);
                    m_event_loop->addEpollEvent(m_fd_event);

                });
                m_event_loop->addEpollEvent(m_fd_event);
                if(!m_event_loop->isLooping()){
                m_event_loop->loop();
                }
            }else{
                ERRORLOG("connect error,errno=%d,error=%s",errno,strerror(errno));
            }
        }
    }

    //异步的发送Message
    //如果发送成功，会调用done函数
    void TcpClient::writeMessage(AbstractProtocol::s_ptr request,std::function<void(AbstractProtocol::s_ptr)>done){

    }

    //异步的读取Message
    //如果读取成功，会调用done函数
    void TcpClient::readMessage(AbstractProtocol::s_ptr request,std::function<void(AbstractProtocol::s_ptr)>done){
        
    }
} // namespace hl
