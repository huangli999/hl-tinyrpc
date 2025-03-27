#ifndef HL_COMMON_MUTEX_H
#define HL_COMMON_MUTEX_H
#include<pthread.h>
namespace hl{

/// @brief 自定義鎖
/// @tparam T 
template<class T>
class ScopeMutex
{
    public:
    ScopeMutex(T&mutex):m_mutex(mutex){
        
        m_mutex.lock();
        m_is_lock=true;
    }

    ~ScopeMutex()
    {
        m_mutex.unlock();
        m_is_lock=false;
    }

    void lock()
    {
        if(!m_is_lock)
        {
            m_mutex.lock();
        }
    }

    void unlock()
    {
        if(m_is_lock)
        {
            m_mutex.unlock();
        }
    }
    private:
    bool m_is_lock{false};
    T&m_mutex;
};
//使用綫程庫獲取互踩鎖
class Mutex{
public:
    Mutex(){
        pthread_mutex_init(&m_mutex,NULL);
    }

    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock(){
        pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
    pthread_mutex_t* getMutex(){
        return &m_mutex;
    }
private:
pthread_mutex_t m_mutex;
};


}



#endif