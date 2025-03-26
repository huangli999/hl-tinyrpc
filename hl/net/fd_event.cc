#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
#include<string.h>
#include<fcntl.h>

namespace hl{

    FdEvent::FdEvent(){
        memset(&m_listen_event,0,sizeof(m_listen_event));
    }
    FdEvent::FdEvent(int fd):m_fd(fd){
        memset(&m_listen_event,0,sizeof(m_listen_event));
    }
    FdEvent::~FdEvent(){

    }

    /// @brief 将传入的事件处理为回调函数
    /// @param event_type 
    /// @return 
    std::function<void()>  FdEvent::handle(TriggerEvent event_type){
        if(event_type==TriggerEvent::IN_EVENT){
            return m_read_callback;
        }
        else if(event_type==TriggerEvent::OUT_EVENT){
            return m_write_callback;
        }
        else if(event_type==TriggerEvent::ERROR_EVENT){
            return m_error_callback;
        }
        return nullptr;
    }

    /// @brief 
    /// @param event_type 
    /// @param callback 
    void FdEvent::listen(TriggerEvent event_type,std::function<void()>callback,std::function<void()>error_callback/*=nullptr*/){
        if(event_type==TriggerEvent::IN_EVENT)
        {
            m_listen_event.events|=EPOLLIN;
            m_read_callback=callback;
        }else{
            m_listen_event.events|=EPOLLOUT;
            m_write_callback=callback;
        }
        if(m_error_callback==nullptr){
            m_error_callback=error_callback;
        }else{
            m_error_callback=nullptr;
        }
        m_listen_event.data.ptr=this;
    }

    /// @brief 设置为非阻塞
    void FdEvent::setNonBlock(){
        int flag=fcntl(m_fd,F_GETFL,0);
        if(flag&O_NONBLOCK){
            return;
        }

        fcntl(m_fd,F_SETFL,flag|O_NONBLOCK);
    }

    /// @brief 取消事件
    /// @param event_type 
    void FdEvent::cancle(TriggerEvent event_type){
        if(event_type==TriggerEvent::IN_EVENT)
        {
            m_listen_event.events&=(~EPOLLIN);
        }else{
            m_listen_event.events&=(~EPOLLOUT);
        }
    }
}