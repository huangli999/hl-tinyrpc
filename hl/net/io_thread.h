#ifndef HL_NET_IO_THREAD_H
#define HL_NET_IO_THREAD_H

#include<pthread.h>
#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include<semaphore.h>
namespace hl
{
class IOThread{
public:
IOThread();

~IOThread();

EventLoop*getEventLoop();

void start();

void join();

public:

static void*Main(void*arg);


private:
pid_t m_thread_id{-1};
pthread_t m_thread{0};//线程句柄
EventLoop*m_event_loop{NULL};//当前IO线程的loop对象

sem_t m_init_semaphore;//IO线程初始化信号量
sem_t m_start_semaphore;//启动IO线程信号量


};  

} // namespace hl





#endif