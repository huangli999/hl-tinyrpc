#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_acceptor.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include<assert.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<string.h>

namespace hl{

    /// @brief 
    /// @param local_addr 
    TcpAcceptor::TcpAcceptor(NetAddr::s_ptr local_addr):m_local_addr(local_addr){
        if(!local_addr->checkVaild()){
            ERRORLOG("invalid local addr%s",local_addr->toString().c_str());
            exit(0);
        }


        m_family=m_local_addr->getFamily();

        m_listenfd=socket(m_family,SOCK_STREAM,0);

        if(m_family<0){
            ERRORLOG("invalid listen %d",m_listenfd);
            exit(0);
        }
        
        //设置端口为可复用
        int val=1;
        if(setsockopt(m_listenfd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val))!=0){
            ERRORLOG("setsockopt REUSERADDR error,errno=%d,error=%s",errno,strerror(errno));
        }

        //bind
        socklen_t len=m_local_addr->getSockLen();
        if(bind(m_listenfd,m_local_addr->getSockAddr(),len)!=0){
            ERRORLOG("bind  error,errno=%d,error=%s",errno,strerror(errno));
            exit(0);
        }

        //listen
        if(listen(m_listenfd,1000)!=0){
            ERRORLOG("listen  error,errno=%d,error=%s",errno,strerror(errno));
            exit(0);
        }
    }

    TcpAcceptor::~TcpAcceptor(){
       
    }

    /// @brief 获取监听的fd
    /// @return 
    int TcpAcceptor::getListenFd(){

        return m_listenfd;

    }

    /// @brief 
    /// @return 返回监听的客户端fd，和地址
    std::pair<int,NetAddr::s_ptr> TcpAcceptor::accept(){
        if(m_family==AF_INET){
            sockaddr_in client_addr;
            memset(&client_addr,0,sizeof(client_addr));
            socklen_t client_addr_len=sizeof(client_addr_len);

            int client_fd=::accept(m_listenfd,reinterpret_cast<sockaddr*>(&client_addr),&client_addr_len);
            if(client_fd<0){
                ERRORLOG("accept error,errno=%d,error=%s",errno,strerror(errno));
            }
            IPNetAddr::s_ptr peer_addr=std::make_shared<IPNetAddr>(client_addr);
            INFOLOG("A client have accepted succ,peer addr{%s}",peer_addr->toString().c_str());
            return std::make_pair(client_fd,peer_addr);
        }else{
            return std::make_pair(-1,nullptr);
        }
    }
}