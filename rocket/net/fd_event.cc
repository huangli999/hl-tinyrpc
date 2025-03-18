#include <string.h>
#include <fcntl.h>
#include "rocket/net/fd_event.h"
#include "rocket/common/log.h"

namespace rocket{

  Fdevent::Fdevent(int fd){


  }

  Fdevent::~Fdevent(){


  }

  void Fdevent::handler(TriggerEvent event){

    if(event==TriggerEvent::IN_EVENT)
    {
      m_read_callback();
    }else
    {
      m_write_callback();
    }


  }

  void Fdevent::listen(TriggerEvent event_type,std::function<void()>callback){
    if(event_type==TriggerEvent::IN_EVENT)
    {
      m_listen_events.events|=EPOLLIN;
      m_read_callback=callback;
    }
    else
    {
      m_listen_events.events|=EPOLLOUT;
      m_write_callback=callback;
    }
    m_listen_events.data.ptr=this;


  }

}