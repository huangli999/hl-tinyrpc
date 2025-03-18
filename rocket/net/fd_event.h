#ifndef ROCKET_FDEVENT_H
#define ROCKET_FDEVENT_H
#include<sys/epoll.h>
#include<functional>
namespace rocket{
  class FdEvent{
    public:
      FdEvent(int fd);

      ~FdEvent();

      enum TriggerEvent{
        IN_EVENT=EPOLLIN,
        OUT_EVENT=EPOLLOUT,
      };

      void handler(TriggerEvent event_type);

      void listen(TriggerEvent event_type,std::function<void()>callback);

      int getFd()const{

        return m_fd;
      }

      epoll_event getEpollEvent(){
        return m_listen_events;
      }
    private:
      int m_fd{-1};

      epoll_event m_listen_events;


      std::function<void()>m_read_callback;
      std::function<void()>m_write_callback;
  }


}


#endif