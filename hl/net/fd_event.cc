#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
#include<string.h>

namespace hl{

    FdEvent::FdEvent(){
        memset(&m_listen_event,0,sizeof(m_listen_event));
    }
    
    FdEvent::~FdEvent(){

    }

    std::function<void()>  FdEvent::handle(TriggerEvent event_type){
        if(event_type==TriggerEvent::IN_EVENT){
            return m_read_callback;
        }
        else{
            return m_write_callback;
        }
    }

    void FdEvent::listen(TriggerEvent event_type,std::function<void()>callback){
        if(event_type==TriggerEvent::IN_EVENT)
        {
            m_listen_event.events|=EPOLLIN;
            m_read_callback=callback;
        }else{
            m_listen_event.events|=EPOLLOUT;
            m_write_callback=callback;
        }
        m_listen_event.data.ptr=this;
    }

}