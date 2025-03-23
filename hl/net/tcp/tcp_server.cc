#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_server.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
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
        int client_fd=m_acceptor->accept();

        FdEvent client_fd_event(client_fd);
        m_client_counts++;

        //把clientfd添加到任意的IO线程
        // m_io_thread_group->getIOThread()->getEventLoop()->addEpollEvent();
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
