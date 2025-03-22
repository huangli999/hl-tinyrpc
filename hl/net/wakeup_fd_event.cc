#include"/home/hl/hl-tinyrpc/hl/net/wakeup_fd_event.h"
#include<unistd.h>
#include"/home/hl/hl-tinyrpc/hl/common/log.h"

namespace hl
{
    WakeUpFdEvent::WakeUpFdEvent(int fd):FdEvent(fd){

    }

    WakeUpFdEvent::~WakeUpFdEvent(){

    }
    void WakeUpFdEvent::wakeup(){
        char buf[8]={'a'};

        int rt=write(m_fd,buf,8);
        if(rt!=8)
        {
            ERRORLOG("write to wakeup fd less than 8 bytes,fd[%d]",m_fd);
        }
    }
} // namespace hl
