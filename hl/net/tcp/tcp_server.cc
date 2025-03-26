#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_server.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_connection.h"

namespace hl
{
    TcpServer::TcpServer(NetAddr::s_ptr local_addr):m_local_addr(local_addr){

        init();

        INFOLOG("hl TcpServer listen success on[%s]",m_local_addr->toString().c_str());
    }


    void TcpServer::init(){


        m_acceptor=std::make_shared<TcpAcceptor>(m_local_addr);
        
        m_main_event_loop=EventLoop::GetCurrentEventLoop();

        m_io_thread_group=new IOThreadGroup(2);//读取配置文件

        m_listen_fd_event=new FdEvent(m_acceptor->getListenFd());

        //当listenfd可读时调用回调函数
        m_listen_fd_event->listen(FdEvent::IN_EVENT,std::bind(&TcpServer::onAccept,this));

        m_main_event_loop->addEpollEvent(m_listen_fd_event);
    }

    void TcpServer::onAccept(){
        auto re=m_acceptor->accept();
        int client_fd=re.first;
        NetAddr::s_ptr peer_addr=re.second;

        
        m_client_counts++;

        //把clientfd添加到任意的IO线程
        // m_io_thread_group->getIOThread()->getEventLoop()->addEpollEvent();
        IOThread*io_thread=m_io_thread_group->getIOThread();
        TcpConnection::s_ptr connection=std::make_shared<TcpConnection>(io_thread->getEventLoop(),client_fd,128,peer_addr,m_local_addr);
        connection->setState(Connected);

        m_client.insert(connection);
       
        DEBUGLOG("TcpServer succ get client,fd=%d",client_fd);
    }

    TcpServer::~TcpServer(){
        if(m_main_event_loop){
            delete m_main_event_loop;
            m_main_event_loop=NULL;
        }
    }

    /// @brief 开启loop悬环
    void TcpServer::start(){
        m_io_thread_group->start();
        m_main_event_loop->loop();
    }
} // namespace hl
