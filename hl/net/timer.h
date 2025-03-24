#ifndef HL_NET_TIMER_H
#define HL_NET_TIMER_H
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
#include<map>
#include"/home/hl/hl-tinyrpc/hl/net/timer_event.h"
#include"/home/hl/hl-tinyrpc/hl/common/mutex.h"
namespace hl
{
class Timer:public FdEvent{
public:
    Timer();

    ~Timer();

    void addTimerEvent(TimerEvent::s_ptr event);

    void deleteTimerEvent(TimerEvent::s_ptr event);

    void onTimer();//当发生IO事件，eventloop会执行这个函数

private:

void resetArriveTime();

std::multimap<int64_t,TimerEvent::s_ptr>m_pending_events;

Mutex m_mutex;

};
} // namespace hl



#endif