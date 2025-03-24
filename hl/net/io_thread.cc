#include"/home/hl/hl-tinyrpc/hl/net/io_thread.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include<assert.h>

namespace hl{

/// @brief 创建新线程
///使用信号量确保同步
IOThread::IOThread(){
    
    int rt=sem_init(&m_init_semaphore,0,0);

    assert(rt==0);

    rt=sem_init(&m_start_semaphore,0,0);

    assert(rt==0);


    pthread_create(&m_thread,NULL,&IOThread::Main,this);
    //wait,直到新线程执行完Main函数的前置
    sem_wait(&m_init_semaphore);

    DEBUGLOG("IOthread%d create success",m_thread_id);
    
}

IOThread::~IOThread(){
    m_event_loop->stop();
    sem_destroy(&m_init_semaphore);
    sem_destroy(&m_start_semaphore);
    pthread_join(m_thread,NULL);

    if(m_event_loop){
        delete m_event_loop;
        m_event_loop=NULL;
    }
}


void*IOThread::Main(void*arg){
    IOThread*thread=static_cast<IOThread*>(arg);

    thread->m_event_loop=new EventLoop();
    thread->m_thread_id=getThreadId();

    //唤醒等待的线程
    sem_post(&thread->m_init_semaphore);

    DEBUGLOG("IOthread %d succ create wait start",thread->m_thread_id);
    //让IO线程等待直到我们唤醒
    sem_wait(&thread->m_start_semaphore);
    DEBUGLOG("IOthread %d start loop",thread->m_thread_id);
    //线程开启loop循环
    thread->m_event_loop->loop();

    return NULL;
}

/// @brief 获得当前IO线程的对象
/// @return 
EventLoop*IOThread::getEventLoop(){
    return m_event_loop;
}

/// @brief 唤醒创建的线程
void IOThread::start(){
    sem_post(&m_start_semaphore);
}

void IOThread::join(){
    pthread_join(m_thread,NULL);
}
}