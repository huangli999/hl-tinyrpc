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
#include"/home/hl/hl-tinyrpc/hl/net/io_thread.h"
#include"/home/hl/hl-tinyrpc/hl/net/io_thread_group.h"
void test_io_thread(){

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

  int i=0;
  hl::TimerEvent::s_ptr time_event=std::make_shared<hl::TimerEvent>(1000,true,[&i](){
    INFOLOG("trigger timer event,count-%d",i++);
  });
  
  hl::IOThreadGroup io_thread_group(2);
  hl::IOThread*io_thread=io_thread_group.getIOThread();
  io_thread->getEventLoop()->addEpollEvent(&event);
  io_thread->getEventLoop()->addTimeEvent(time_event);

  hl::IOThread*io_thread2=io_thread_group.getIOThread();
  io_thread2->getEventLoop()->addTimeEvent(time_event);

  io_thread_group.start();
  io_thread_group.join();



}
int main() {

  hl::Config::SetGlobalConfig("/home/hl/hl-tinyrpc/conf/hl.xml");
  hl::Logger::InitGlobalLogger();

  test_io_thread();
  // int listenfd=socket(AF_INET,SOCK_STREAM,0);
  // if(listenfd==-1){
  //   ERRORLOG("listen==-1",NULL);
  //   exit(0);
  // }
  // sockaddr_in addr;
  // memset(&addr,0,sizeof(addr));

  // addr.sin_port=htons(12346);
  // addr.sin_family=AF_INET;
  // inet_aton("127.0.0.1",&addr.sin_addr);
  // int rt=bind(listenfd,reinterpret_cast<sockaddr*>(&addr),sizeof(addr));
  // if(rt!=0)
  // {
  //   ERRORLOG("bind error",NULL);
  //   exit(1);
  // }

  // rt=listen(listenfd,100);
  // if(rt!=0)
  // {
  //   ERRORLOG("listen error",NULL);
  //   exit(1);

  // }

  // hl::FdEvent event(listenfd);
  // event.listen(hl::FdEvent::IN_EVENT,[listenfd](){
  //   sockaddr_in peer_addr;
  //   socklen_t addr_len=sizeof(peer_addr);
  //   memset(&peer_addr,0,sizeof(peer_addr));
  //   int clientfd=accept(listenfd,reinterpret_cast<sockaddr*>(&peer_addr),&addr_len);

  //   ;
  //   DEBUGLOG("succ get client fd[%d],peer addr: [%s:%d]",clientfd,inet_ntoa(peer_addr.sin_addr),ntohs(peer_addr.sin_port));
  // });
  // eventloop->addEpollEvent(&event);
  // int i=0;
  // hl::TimerEvent::s_ptr time_event=std::make_shared<hl::TimerEvent>(100,true,[&i](){
  //   INFOLOG("trigger timer event,count-%d",i++);
  // });
  // eventloop->addTimeEvent(time_event);
  // eventloop->loop();
  return 0;
}