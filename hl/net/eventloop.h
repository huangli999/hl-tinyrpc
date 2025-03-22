#ifndef HL_NET_EVENTLOOP_H
#define HL_NET_EVENTLOOP_H
#include<functional>
#include<set>
#include<pthread.h>
#include<queue>
#include"/home/hl/hl-tinyrpc/hl/common/mutex.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
#include"/home/hl/hl-tinyrpc/hl/net/wakeup_fd_event.h"

namespace hl{

class EventLoop{

public:
    EventLoop();

    ~EventLoop();

    void loop();

    void wakeup();

    void stop();

    void dealWakeUp();

    void addEpollEvent(FdEvent*event);

    void deleteEpollEvent(FdEvent*event);

    bool isInLoopThread();

    void addTask(std::function<void()>cb,bool is_wake_up=false);

    void initWakeUpFdEvent();

private:
    pid_t m_thread_id{0};

    std::set<int>m_listen_fds;

    int m_epoll_fd{0};

    int m_wakeup_fd{0};

    WakeUpFdEvent *m_wakeup_fd_event{NULL};

    bool m_stop_flag{false};

    std::queue<std::function<void()>>m_pending_tasks;//io线程的任务队列

    Mutex m_mutex;
};




}


#endif