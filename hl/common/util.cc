#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include<unistd.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<sys/time.h>
#include<string.h>
#include<arpa/inet.h>
namespace hl{

static int g_pid=0;

static thread_local int g_thread_id=0;

pid_t getPid(){

    if(g_pid!=0)
    {
        return g_pid;
    }
    return getpid();
}

pid_t getThreadId(){

    if(g_thread_id!=0)
    {
        return g_thread_id;
    }
    return syscall(SYS_gettid);
}

/// @brief 
/// tv_sec秒数，tv_usec微妙数
/// @return 
int64_t getNowMs(){
    timeval val;
    gettimeofday(&val,NULL);
    return val.tv_sec*1000+val.tv_usec/1000;
}

int32_t getInt32FromNetByte(const char*buf){
    int32_t re;
    memcpy(&re,buf,sizeof(re));
    return ntohl(re);
}


}