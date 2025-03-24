#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/net/timer_event.h"

namespace hl
{
    TimerEvent::TimerEvent(int interval,bool is_repeated,std::function<void()>cb):m_interval(interval),m_is_repeated(is_repeated),m_task(cb){
        resetArriveTime();
    }


    /// @brief 重设时间戳
    void TimerEvent::resetArriveTime(){

        m_arrive_time=getNowMs()+m_interval;  
        DEBUGLOG("success create time event,will excute at {%lld}",m_arrive_time);
    }
} // namespace hl
