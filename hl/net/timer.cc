#include"/home/hl/hl-tinyrpc/hl/net/timer.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include<sys/timerfd.h>
#include<string.h>
namespace hl{

    Timer::Timer():FdEvent(){
        m_fd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);

        DEBUGLOG("timer fd=%d",m_fd);

        listen(FdEvent::IN_EVENT,std::bind(&Timer::onTimer,this));//监听定时器fd
    }

    Timer::~Timer(){

    }

    void Timer::addTimerEvent(TimerEvent::s_ptr event){
        bool is_reset_timerfd=false;
        ScopeMutex<Mutex>lock(m_mutex);
        if(m_pending_events.empty())
        {
            is_reset_timerfd=true;
        }else{
            auto it=m_pending_events.begin();
            if((*it).second->getArriveTime()>event->getArriveTime()){
                is_reset_timerfd=true;
            }
        }
        m_pending_events.emplace(event->getArriveTime(),event);
        lock.unlock();
        if(is_reset_timerfd){
            resetArriveTime();
        }
    }

    void Timer::deleteTimerEvent(TimerEvent::s_ptr event){
        event->setCancle(true);

        ScopeMutex<Mutex>lock(m_mutex);

        auto begin=m_pending_events.lower_bound(event->getArriveTime());

        auto end=m_pending_events.upper_bound(event->getArriveTime());

        auto it=begin;

        for(it=begin;it!=end;++it){
            if(it->second==event)
            {
                break;
            }
        }
        if(it!=end){
            m_pending_events.erase(it);
        }
        lock.unlock();
        DEBUGLOG("succ delete TimerEvent at arrive time %lld",event->getArriveTime());
    }

    void Timer::onTimer(){
        //处理缓冲区数据，防止下一次继续触发可读事件
        char buf[8];
        while(1){
            if(read(m_fd,buf,8)==-1&&errno==EAGAIN){
                break;
            }
        }
        //执行定时任务
        int64_t now=getNowMs();//获取当前时间戳
        std::vector<TimerEvent::s_ptr>tmps;//定时器数组
        std::vector<std::pair<int64_t,std::function<void()>>>tasks;//定时器数组
        ScopeMutex<Mutex>lock(m_mutex);
        auto it=m_pending_events.begin();
        for(it=m_pending_events.begin();it!=m_pending_events.end();++it){
            if((*it).first<=now){
                if(!(*it).second->isCancle()){
                    tmps.push_back((*it).second);
                    tasks.push_back(std::make_pair((*it).second->getArriveTime(),(*it).second->getCallBack()));
                }else{
                    break;
                }

            }
        }
        m_pending_events.erase(m_pending_events.begin(),it);
        lock.unlock();

        //需要把重复的任务再次添加
        for(auto i=tmps.begin();i!=tmps.end();++i){
            if((*i)->isRepeated()){
                (*i)->resetArriveTime();
                addTimerEvent(*i);
            }
        }

        resetArriveTime();
        for(auto i:tasks){
            if(i.second){
                i.second();
            }
        }
    }

    void Timer::resetArriveTime(){
        ScopeMutex<Mutex>lock(m_mutex);
        auto tmp=m_pending_events;
        lock.unlock();
        if(tmp.size()==0)
        {   
            return;
        }
        int64_t now=getNowMs();
        auto it=tmp.begin();
        int64_t interval=0;
        if(it->second->getArriveTime()>now){
            interval=it->second->getArriveTime()-now;
        }else{
            interval=100;//100ms
        }
        timespec ts;
        memset(&ts,0,sizeof(ts));
        ts.tv_sec=interval/1000;//ms
        ts.tv_nsec=(interval%1000)*1000000;//ms

        itimerspec value;
        memset(&value,0,sizeof(value));
        value.it_value=ts;

        int rt=timerfd_settime(m_fd,0,&value,NULL);
        if(rt!=0){
            ERRORLOG("timerfd_settime,error,error=%d,error info[%s]",errno,strerror(errno));
        }
        DEBUGLOG("timer reset to %lld",now+interval);
    }
}