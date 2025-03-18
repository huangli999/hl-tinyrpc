#ifndef ROCKET_NET_EVENTLOOP_H
#define ROCKET_NET_EVENTLOOP_H

#include<pthread.h>
#include<set>
#include<functional>
#include<queue>
#include"rocket/common/mutex.h"
#include"rocket/net/fd_event.h"
namespace rocket{
   
  class EventLoop{
    public:
      EventLoop();
      
      ~EventLoop();

      void loop();

      void wakeup();

      void stop();

      bool isInLoopThread();

      void addTask(std::function<void()>cb,bool is_wake_up=false);
    private:
      void dealWakeup();

      void addEpollEvent(FdEvent*event);

      void deleteEpollEvent(FdEvent*event);
    private:

      pid_t m_thread_id{0};//线程id

      int m_wakeup_fd{0};


      int m_epoll_fd{0};//epoll句柄

      bool m_stop_flag{false};

      std::set<int>m_listen_fds;

      std::queue<std::function<void()>>m_pending_tasks;

    
   } 







}
  




#endif