#include"/home/hl/hl-tinyrpc/hl/net/fd_event_group.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/mutex.h"
namespace hl{

    static FdEventGroup*g_fd_event_group=NULL;

    /// @brief 初始化事件
    /// @param size 
    FdEventGroup::FdEventGroup(int size):m_size(size){
        for(int i=0;i<m_size;++i){
            m_fd_group.push_back(new FdEvent(i));
        }
    }

    FdEventGroup::~FdEventGroup(){
        for(int i=0;i<m_size;++i){
            if(m_fd_group[i]!=NULL){
                delete m_fd_group[i];
                m_fd_group[i]=NULL;
            }
        }
    }

    /// @brief 获得事件组的对象
    /// @return 
    FdEventGroup*FdEventGroup::GetFdEventGroup(){
        if(g_fd_event_group!=NULL){
            return g_fd_event_group;
        }
        g_fd_event_group=new FdEventGroup(128);
        return g_fd_event_group;
    }

    /// @brief 获得指定文件描述符事件
    /// @param fd 
    /// @return 
    FdEvent*FdEventGroup::getFdEvent(int fd){
        ScopeMutex<Mutex>lock(m_mutex);
        if(fd<(int)m_fd_group.size()){
            return m_fd_group[fd];
        }
        int new_size=(int)(fd*1.5);
        for(int i=m_fd_group.size();i<new_size;++i){
            m_fd_group.push_back(new FdEvent(i));
        }
        return m_fd_group[fd];
    }
}