#ifndef HL_NET_FD_EVENT_GROUP_H
#define HL_NET_FD_EVENT_GROUP_H
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
#include"/home/hl/hl-tinyrpc/hl/common/mutex.h"
#include<vector>
namespace hl{

class FdEventGroup{

public:

FdEventGroup(int size);

~FdEventGroup();

FdEvent*getFdEvent(int fd);
public:

static FdEventGroup*GetFdEventGroup();

private:
int m_size{0};
std::vector<FdEvent*>m_fd_group;

Mutex m_mutex;


};



}





#endif