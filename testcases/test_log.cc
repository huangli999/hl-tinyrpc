
#include<iostream>
#include<pthread.h>
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/config.h"
void *func(void*)
{
  int i=20;
  while(i-->0){
    DEBUGLOG("debug this is thread in %s","func");
    INFOLOG("info this is thread in %s","func");
  }

  return NULL;
}


int main() {
  hl::Config::SetGlobalConfig("/home/hl/hl-tinyrpc/conf/hl.xml");
  hl::Logger::InitGlobalLogger();
  pthread_t thread;
  pthread_create(&thread,NULL,&func,NULL);

  int i=20;
  while(i-->0)
  {
    DEBUGLOG("test debug log %s","11");
    INFOLOG("test info log %s","l1");
  }
  pthread_join(thread,NULL);
  return 0;
}