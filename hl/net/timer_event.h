#ifndef HL_NET_TIMER_EVENT_H
#define HL_NET_TIMER_EVENT_H

#include<memory>
#include<functional>

namespace hl
{
class TimerEvent{
public:

typedef std::shared_ptr<TimerEvent>s_ptr;

TimerEvent(int interval,bool is_repeated,std::function<void()>cb);

int64_t getArriveTime()const{
    return m_arrive_time;
}

void setCancle(bool value){
    m_is_cancled=value;
}

bool isCancle(){
    return m_is_cancled;
}

bool isRepeated(){
    return m_is_repeated;
}

std::function<void()>getCallBack(){
    return m_task;
}

void resetArriveTime();

private:
int64_t m_arrive_time;
int64_t m_interval;
bool m_is_repeated{false};
bool m_is_cancled{false};

std::function<void()>m_task;
};


} // namespace hl





#endif