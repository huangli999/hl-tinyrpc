#ifndef HL_NET_FD_EVENT_H
#define HL_NET_FD_EVENT_H
#include<functional>
#include<sys/epoll.h>
namespace hl{
class FdEvent{

public:
enum TriggerEvent{
    IN_EVENT=EPOLLIN,
    OUT_EVENT=EPOLLOUT,
};
    FdEvent();
    FdEvent(int fd);
    
    ~FdEvent();

    void setNonBlock();

    std::function<void()> handle(TriggerEvent event_type);

    void listen(TriggerEvent event_type,std::function<void()>callback);

    //取消监听
    void cancle(TriggerEvent event_type);

    int getFd()const{
        return m_fd;
    }

    epoll_event getEpollEvent(){
        return m_listen_event;
    }
protected:
int m_fd{-1};

epoll_event m_listen_event;
std::function<void()>m_read_callback;
std::function<void()>m_write_callback;
};


}


#endif