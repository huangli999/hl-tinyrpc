#include<sys/socket.h>
#include <sys/epoll.h>
#include<sys/eventfd.h>
#include"rocket/net/eventloop.h"
#include"rocket/common/log.h"
#include"rocket/common/util.h"
#include<string.h>


#define ADD_TO_EPOLL() \
    auto it=m_listen_fds.find(event->getFd());\
    int op=EPOLL_CTL_ADD;\
    if(it!=m_listen_fds.end()){\
      op=EPOLL_CTL_MOD;\
    }\
    epoll_event tmp=event->getEpollEvent();\
    int rt=epoll_ctl(m_epoll_fd,op,event->getFd(),&tmp);\
    if(rt==-1){\
      ERRORLOG("failed epoll_ctl when add fd,error=%d,error=%s",errno,strerror(errno));\
    }\
    DEBUGLOG("add event sucess,fd[%d]",event->getFd());\

#define DEL_TO_EPOLL()\
    auto it=m_listen_fds.find(event->getFd());\
    if(it==m_listen_fds.end()){\
      return;\
    }\
    int op=EPOLL_CTL_DEL;\
    epoll_event tmp=event->getEpollEvent();\
    int rt=epoll_ctl(m_epoll_fd,op,event->getFd(),&tmp);\
    if(rt==-1){\
      ERRORLOG("failed epoll_ctl when delete fd,error=%d,error=%s",errno,strerror(errno));\
    }\
    DEBUGLOG("delete event success,fd[%d]",event->getFd())\

namespace rocket{

  //
  static thread_local EventLoop*t_current_eventloop=NULL;

  static int g_epoll_max_timeout=10000;

  static int g_epoll_max_events=10;


  EventLoop::EventLoop(){
    if(t_current_eventloop!=NULL)
    {
      ERRORLOG("failed to create event loop,this thread has created event loop");
      exit(0);
    }

    m_thread_id=getThreadId();

    m_epoll_fd=epoll_create(10);

    if(m_epoll_fd==-1)
    {
      ERRORLOG("failed to create event loop,epoll_create error,error info[%d]",errno);
      exit(0);
    }

    m_wakeup_fd=eventfd(0,EFD_NONBLOCK);

    if(m_wakeup_fd<0)
    {
      ERRORLOG("failed to create event loop,eventfd error,error info[%d]",errno);
      exit(0);
    }

    epoll_event event;

    event.events=EPOLLIN;

    int rt=epoll_ctl(m_epoll_fd,EPOLL_CTL_ADD,m_wakeup_fd,&event);

    if(rt==-1)
    {
      ERRORLOG("failed to create event loop,epoll_ctl error,error info[%d]",errno);
      exit(0);
    }

    INFOLOG("succ create event loop in thread %d",m_thread_id);

    t_current_eventloop=this;

  }
      
  EventLoop::~EventLoop(){



  }

  void EventLoop::loop(){
    while(!m_stop_flag)
    {
      ScopeMutex<Mutex>lock(m_mutex);//加锁
      std::queue<std::function<void()>>tmp_tasks;
      m_pending_tasks.swap(tmp_tasks);
      lock.unlock();

      while(!tmp_tasks.empty()&&!m_stop_flag)
      {
        tmp_tasks.front()();
        tmp_tasks.pop();
      }

      int timeout=g_epoll_max_timeout;
      epoll_event result_events[g_epoll_max_events];
      int rt=epoll_wait(m_epoll_fd,result_events,g_epoll_max_events,timeout);

      if(rt<0)
      {
        ERRORLOG("epoll_wait error,error=",errno);

      }
      else{
        for(int i=0;i<rt;i++)
        {
          epoll_event trigger_event=result_events[i];
          if(trigger_event.events|EPOLLIN)
          {
              FdEvent*fd_event=static_cast<FdEvent*>(trigger_event.data.ptr);
              if(fd_event)
              {

              }
          }


        }

      }

    }


  }

  void EventLoop::wakeup(){



  }

  void EventLoop::stop(){
    m_stop_flag=true;
  }

  void EventLoop::addEpollEvent(FdEvent*event){
    if(isInLoopThread()){
      ADD_TO_EPOLL();
    }else {
      auto cb=[this,event](){
        ADD_TO_EPOLL();
      };
      addTask(cb,true);
    }
  }

  void EventLoop::deleteEpollEvent(FdEvent*event){
    if(isInLoopThread()){
        DEL_TO_EPOLL();
    }else {
      auto cb=[this,event](){
        DEL_TO_EPOLL();
      };
      addTask(cb,true);
    }
  }
  void EventLoop::dealWakeup(){

  }

  bool EventLoop::isInLoopThread()
  {
    return getThreadId()==m_thread_id;
  }

  void EventLoop::addTask(std::function<void()>cb,bool is_wake_up/*=false*/){
    ScopeMutex<Mutex>lock(&m_mutex);
    m_pending_tasks.push(cb);
    lock.unlock();
    if(is_wake_up)
    {
      wakeup();
    }
  }

}




