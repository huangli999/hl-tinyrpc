#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_client.h"
#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include<sys/socket.h>
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event_group.h"
#include"/home/hl/hl-tinyrpc/hl/common/error_code.h"
#include<unistd.h>
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
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
        m_connection=std::make_shared<TcpConnection>(m_event_loop,m_fd,128,peer_addr,nullptr,TcpConnectionByClient);
        m_connection->setConnectionType(TcpConnectionByClient);
    }

    TcpClient::~TcpClient(){
        if(m_fd>0){
            close(m_fd);
        }
    }

    //异步的进行connect
    //connect完成，done会被执行
    void  TcpClient::connect(std::function<void()>done){
        int rt=::connect(m_fd,m_peer_addr->getSockAddr(),m_peer_addr->getSockLen());
        if(rt==0){
            initLocalAddr();
            DEBUGLOG("connect[%s]success",m_peer_addr->toString().c_str());
            m_connection->setState(Connected);
            if(done){
                done();
            }
            
        }else if(rt==-1){
            if(errno==EINPROGRESS){
                m_fd_event->listen(FdEvent::OUT_EVENT,
                    [this,done](){



                int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
                if ((rt < 0 && errno == EISCONN) || (rt == 0)) {
                    DEBUGLOG("connect [%s] sussess", m_peer_addr->toString().c_str());
                    initLocalAddr();
                    m_connection->setState(Connected);
                } else {
                    if (errno == ECONNREFUSED) {
                    m_connect_errcode = ERROR_PEER_CLOSE;
                    m_connect_error_info = "connect refused, sys error = " + std::string(strerror(errno));
                    } else {
                    m_connect_errcode = ERROR_FAILED_CONNECT;
                    m_connect_error_info = "connect unkonwn error, sys error = " + std::string(strerror(errno));
                    }
                    ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno));
                    close(m_fd);
                    m_fd = socket(m_peer_addr->getFamily(), SOCK_STREAM, 0);
                }

                    //连接完去掉可写事件监听
                    m_event_loop->deleteEpollEvent(m_fd_event);
                    //如果连接成功，才会执行回调函数
                    if(done){
                        
                        done();
                    }

                });
                m_event_loop->addEpollEvent(m_fd_event);
                if(!m_event_loop->isLooping()){
                    m_event_loop->loop();
                }
            }else{
                ERRORLOG("connect error,errno=%d,error=%s",errno,strerror(errno));
                m_connect_errcode=ERROR_FAILED_CONNECT;
                m_connect_error_info="connect error,sys error="+std::string(strerror(errno));
                if(done){
                        
                    done();
                }
            }
        }
    }

    //异步的发送Message
    //如果发送成功，会调用done函数
    void TcpClient::writeMessage(AbstractProtocol::s_ptr message,std::function<void(AbstractProtocol::s_ptr)>done){
        //1、把message对象写入connection的buffer
        //2、启动connection可写事件
        m_connection->pushSendMessage(message,done);
        m_connection->listenWrite();

    }

    //异步的读取Message
    //如果读取成功，会调用done函数
    void TcpClient::readMessage(const std::string &msg_id,std::function<void(AbstractProtocol::s_ptr)>done){
        //监听可读事件
        //从buffer里encode得到message
        m_connection->pushReadMessage(msg_id,done);
        m_connection->listenRead();
    }

    void TcpClient::stop(){
        if(m_event_loop->isLooping()){
            m_event_loop->stop();
        }
    }

    int TcpClient::getConnectErrorCode(){
        return m_connect_errcode;
    }

    std::string TcpClient::getConnectErrorInfo(){
        return m_connect_error_info;
    }


    NetAddr::s_ptr TcpClient::getPeerAddr(){
        return m_peer_addr;
    }

    NetAddr::s_ptr TcpClient::getLocalAddr(){
        return m_local_addr;
    }

    void TcpClient::initLocalAddr(){
        sockaddr_in local_addr;
        socklen_t len=sizeof(local_addr);
        int ret=getsockname(m_fd,reinterpret_cast<sockaddr*>(&local_addr),&len);
        if(ret!=0){
            ERRORLOG("initLocalAddr error,getsockname error,errno=%d,error=%s",errno,strerror(errno));
            return;
        }
        m_local_addr=std::make_shared<IPNetAddr>(local_addr);
    }
} // namespace hl
