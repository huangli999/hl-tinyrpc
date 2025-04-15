#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/eventfd.h>
#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include<string.h>

///添加到epoll中
//先检查是否已添加，使用set
#define ADD_TO_EPOLL()  \
    auto it=m_listen_fds.find(event->getFd());\
    int op=EPOLL_CTL_ADD;\
    if(it!=m_listen_fds.end())\
    {\
        op=EPOLL_CTL_MOD;\
    }\
    epoll_event tmp=event->getEpollEvent();\
    int rt=epoll_ctl(m_epoll_fd,op,event->getFd(),&tmp);\
    if(rt==-1){\
        ERRORLOG("failed epoll_ctl when add fd,errno=%d,error info=%s",errno,strerror(errno));\
    }\
    m_listen_fds.insert(event->getFd());\
    DEBUGLOG("succ add event fd,fd{%d}",event->getFd());\

#define DEL_TO_EPOLL() \
    auto it=m_listen_fds.find(event->getFd());\
    if(it==m_listen_fds.end())\
    {\
        return;\
    }\
    int op=EPOLL_CTL_DEL;\
    epoll_event tmp=event->getEpollEvent();\
    int rt=epoll_ctl(m_epoll_fd,op,event->getFd(),&tmp);\
    if(rt==-1)\
    {\
        ERRORLOG("failed epoll_ctl when del fd,errno=%d,error info=%s",errno,strerror(errno));\
    }\
    m_listen_fds.erase(event->getFd());\
    DEBUGLOG("succ del event fd,fd{%d}",event->getFd());\

namespace hl{

    static thread_local EventLoop*t_current_eventloop=NULL;

    static int g_epoll_max_timeout=10000;

    static int g_epoll_max_events=10;

    /// @brief 获取线程id，eventloop只使用一个线程
    EventLoop::EventLoop(){
        if(t_current_eventloop!=NULL)
        {
            ERRORLOG("failed to create event loop,this thread has created event loop,error info[%d]",errno);
            exit(0);
        }
        m_thread_id=getThreadId();

        m_epoll_fd=epoll_create(10);//创建epoll句柄


        if(m_epoll_fd==-1)
        {
            ERRORLOG("failed to create event loop,epoll_create error,error info[%d]",errno);
            exit(0);
        }


        initWakeUpFdEvent();
        initTimer();
        INFOLOG("succ create eventloop in thread %d",m_thread_id);
        t_current_eventloop=this;
    }

    /// @brief 
    EventLoop::~EventLoop(){
        close(m_epoll_fd);
        if(m_wakeup_fd_event)
        {
            delete m_wakeup_fd_event;
            m_wakeup_fd_event=NULL;
        }
        if(m_timer)
        {
            delete m_timer;
            m_timer=NULL;
        }
    }


    void EventLoop::initTimer(){
        m_timer=new Timer();
        addEpollEvent(m_timer);
    }

    void EventLoop::addTimeEvent(TimerEvent::s_ptr event){
        m_timer->addTimerEvent(event);
    }

    /// @brief 初始化唤醒fd事件
    void EventLoop::initWakeUpFdEvent(){
        m_wakeup_fd=eventfd(0,EFD_NONBLOCK);
        if(m_wakeup_fd<0)
        {
            ERRORLOG("failed to create event loop,eventfd create error,error info[%d]",errno);
            exit(0);
        }
        m_wakeup_fd_event=new WakeUpFdEvent(m_wakeup_fd);

        m_wakeup_fd_event->listen(FdEvent::IN_EVENT,[this](){
            char buf[8];

            while(read(m_wakeup_fd,buf,8)!=-1&&errno!=EAGAIN)
            {

            }
            DEBUGLOG("read full bytes from wakeup fd[%d]",m_wakeup_fd);
        });
        addEpollEvent(m_wakeup_fd_event);

    }


    /// @brief 
    void EventLoop::loop(){
        
        m_is_looping=true;
        while(!m_stop_flag)
        {
            ScopeMutex<Mutex>lock(m_mutex);
            std::queue<std::function<void()>>tmp_tasks;
            m_pending_tasks.swap(tmp_tasks);
            lock.unlock();
            //执行具体的任务
            while(!tmp_tasks.empty())
            {
                std::function<void()>cb=tmp_tasks.front();
                tmp_tasks.pop();
                if(cb)
                {
                    cb();
                }
            }
            //如果有定时器需要执行，那么执行
            //1、怎么判断一个定时任务需要执行，当（now()>arrive_time)
            //2、arrive_time如何让eventloop监听


            int timeout=g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events];
            // DEBUGLOG("now begin epoll_wait",NULL);
            
            //返回的新触发IO事件的socket个数，将新的待处理事件放入resul_event中
            int rt=epoll_wait(m_epoll_fd,result_events,g_epoll_max_events,timeout);
            DEBUGLOG("now end epoll_wait,rt=%d",rt);

            if(rt<0)
            {
                ERRORLOG("epoll_wait error,errno=",errno);

            }else{
                for(int i=0;i<rt;++i)
                {
                    epoll_event trigger_event=result_events[i];
                    FdEvent*fd_event=static_cast<FdEvent*>(trigger_event.data.ptr);
                    if(fd_event==NULL){
                        ERRORLOG("fd_event = NULL, continue",NULL);
                        continue;
                    }
                    if(trigger_event.events&EPOLLIN)
                    {
                        DEBUGLOG("fd%d trigger EPOLLIN event",fd_event->getFd());
                        addTask(fd_event->handle(FdEvent::IN_EVENT));
                    }
                    if(trigger_event.events&EPOLLOUT)
                    {
                        DEBUGLOG("fd%d trigger EPOLLOUT event",fd_event->getFd());
                        addTask(fd_event->handle(FdEvent::OUT_EVENT));
                    }
                    if(trigger_event.events&EPOLLERR)
                    {
                        DEBUGLOG("fd%d trigger EPOLLERR event",fd_event->getFd());
                        deleteEpollEvent(fd_event);
                        if(fd_event->handle(FdEvent::ERROR_EVENT)!=nullptr){
                        addTask(fd_event->handle(FdEvent::ERROR_EVENT));
                        }
                    }
                }
            }

        }
    }

    //唤醒
    void EventLoop::wakeup(){
        INFOLOG("WAKE UP",NULL);
        m_wakeup_fd_event->wakeup();
    }

    void EventLoop::stop(){
        m_stop_flag = true;
        wakeup();
    }

    void EventLoop::dealWakeUp(){

    }

    /// @brief 添加事件 如果是主线程则直接添加，否则生成回调函数，放入任务队列中
    /// @param event 
    void EventLoop::addEpollEvent(FdEvent*event){
        if(isInLoopThread())
        {
            ADD_TO_EPOLL();
        }else{
            auto cb=[this,event](){
                ADD_TO_EPOLL();

            };
            addTask(cb,true);
        }

    }

    void EventLoop::deleteEpollEvent(FdEvent*event){
        if(isInLoopThread())
        {
            DEL_TO_EPOLL();
        }
        else
        {
            auto cb=[this,event]()
            {
                DEL_TO_EPOLL();
            };
            addTask(cb,true);
        }

    }

    /// @brief 判断当前线程是不是主线程
    /// @return 
    bool EventLoop::isInLoopThread(){
        return getThreadId()==m_thread_id;
    }

    /// @brief 将回调函数放入任务队列
    /// @param cb 
    /// @param is_wake_up 
    void EventLoop::addTask(std::function<void()>cb,bool is_wake_up){
        ScopeMutex<Mutex>lock(m_mutex);
        m_pending_tasks.push(cb);
        lock.unlock();
        if(is_wake_up)
        {
            wakeup();
        }
    }

    EventLoop*EventLoop::GetCurrentEventLoop(){

        if(t_current_eventloop){
            return t_current_eventloop;
        }
        t_current_eventloop=new EventLoop();
        return t_current_eventloop;

    }

    bool EventLoop::isLooping(){
        return m_is_looping;
    }


}