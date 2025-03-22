#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <memory>
#include<sys/socket.h>

#include<iostream>
#include<pthread.h>
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include"/home/hl/hl-tinyrpc/hl/common/config.h"
#include"/home/hl/hl-tinyrpc/hl/net/fd_event.h"
#include"/home/hl/hl-tinyrpc/hl/net/eventloop.h"
#include"/home/hl/hl-tinyrpc/hl/net/timer_event.h"

int main() {

  hl::Config::SetGlobalConfig("/home/hl/hl-tinyrpc/conf/hl.xml");
  hl::Logger::InitGlobalLogger();
  hl::EventLoop*eventloop=new hl::EventLoop();

  int listenfd=socket(AF_INET,SOCK_STREAM,0);
  if(listenfd==-1){
    ERRORLOG("listen==-1",NULL);
    exit(0);
  }
  sockaddr_in addr;
  memset(&addr,0,sizeof(addr));

  addr.sin_port=htons(12346);
  addr.sin_family=AF_INET;
  inet_aton("127.0.0.1",&addr.sin_addr);
  int rt=bind(listenfd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr));
  if(rt!=0)
  {
    ERRORLOG("bind error",NULL);
    exit(1);
  }

  rt=listen(listenfd,100);
  if(rt!=0)
  {
    ERRORLOG("listen error",NULL);
    exit(1);

  }

  hl::FdEvent event(listenfd);
  event.listen(hl::FdEvent::IN_EVENT,[listenfd](){
    sockaddr_in peer_addr;
    socklen_t addr_len=sizeof(peer_addr);
    memset(&peer_addr,0,sizeof(peer_addr));
    int clientfd=accept(listenfd,reinterpret_cast<sockaddr*>(&peer_addr),&addr_len);

    ;
    DEBUGLOG("succ get client fd[%d],peer addr: [%s:%d]",clientfd,inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
  });
  eventloop->addEpollEvent(&event);
  int i=0;
  hl::TimerEvent::s_ptr time_event=std::make_shared<hl::TimerEvent>(100,true,[&i](){
    INFOLOG("trigger timer event,count-%d",i++);
  });
  eventloop->addTimeEvent(time_event);
  eventloop->loop();
  return 0;
}