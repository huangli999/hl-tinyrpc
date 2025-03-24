#include"/home/hl/hl-tinyrpc/hl/net/io_thread_group.h"

namespace hl
{
    /// @brief 初始化线程
    /// @param size 
    IOThreadGroup::IOThreadGroup(int size):m_size(size){
        m_io_thread_groups.resize(m_size);
        for(int i=0;i<m_size;++i){
            m_io_thread_groups[i]=new IOThread();

        }
    }

    IOThreadGroup::~IOThreadGroup(){

    }
    //开启所有io线程
    void IOThreadGroup::start(){
        for(size_t i=0;i<m_io_thread_groups.size();++i){
            m_io_thread_groups[i]->start();
        }
    }

    void IOThreadGroup::join(){
        for(size_t i=0;i<m_io_thread_groups.size();++i){
            m_io_thread_groups[i]->join();
        }
    }

    /// @brief 使用轮询获取io线程
    /// @return 
    IOThread*IOThreadGroup::getIOThread(){
    if(m_index==(int)m_io_thread_groups.size()||m_size==-1){
        m_index=0;
    }
    return m_io_thread_groups[m_index++];
    }
} // namespace hl
