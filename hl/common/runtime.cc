#include"/home/hl/hl-tinyrpc/hl/common/runtime.h"

namespace hl{
    thread_local RunTime*t_run_time=NULL;

    RunTime*RunTime::GetRunTime(){
        if(t_run_time){
            return t_run_time;
        }
        t_run_time=new RunTime();
        return t_run_time;
    }
}