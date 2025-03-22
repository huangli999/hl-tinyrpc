#ifndef HL_NET_FD_WAKEUP_FD_EVENT_H
#define HL_NET_FD_WAKEUP_FD_EVENT_H

#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
namespace hl{

class WakeUpFdEvent:public FdEvent{
public:

WakeUpFdEvent(int fd);

~WakeUpFdEvent();

void wakeup();

private:
};


}


#endif