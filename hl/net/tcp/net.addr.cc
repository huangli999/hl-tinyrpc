#include"/home/hl/hl-tinyrpc/hl/net/tcp/net_addr.h"
#include<string.h>
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
namespace hl{

    IPNetAddr::IPNetAddr(const std::string&ip,uint16_t port):m_ip(ip),m_port(port){
        memset(&m_addr,0,sizeof(m_addr));
        
        m_addr.sin_family=AF_INET;
        m_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
        m_addr.sin_port=htons(m_port);//主机字节序转为网络字节序
    }

    IPNetAddr::IPNetAddr(const std::string&addr){
        size_t i=addr.find_first_of(":");
        if(i==addr.npos){
            ERRORLOG("invalid ipv4 addr%s",addr.c_str());
            return;
        }
        m_ip=addr.substr(0,i);
        m_port=std::atoi(addr.substr(i+1,addr.size()-i-1).c_str());
        memset(&m_addr,0,sizeof(m_addr));

        m_addr.sin_family=AF_INET;
        m_addr.sin_addr.s_addr=inet_addr(m_ip.c_str());
        m_addr.sin_port=htons(m_port);//主机字节序转为网络字节序
    }

    IPNetAddr::IPNetAddr(sockaddr_in addr):m_addr(addr){
        m_ip=inet_ntoa(m_addr.sin_addr);//将网络字节序转ASICIL
        m_port=ntohs(m_addr.sin_port);
    }


    sockaddr*IPNetAddr::getSockAddr(){
        return reinterpret_cast<sockaddr*>(&m_addr);
    }

    socklen_t IPNetAddr::getSockLen(){
        return sizeof(m_addr);
    }

    /// @brief 设置协议为IPV4
    /// @return 
    int IPNetAddr::getFamily(){
        return AF_INET;
    }

    /// @brief 返回ip地址和端口号的字符串
    /// @return 
    std::string IPNetAddr::toString(){
        std::string res;
        res=m_ip+":"+std::to_string(m_port);
        return res;
    }
    
    /// @brief 检查端口号是否有效
    /// @return 
    bool IPNetAddr::checkVaild(){
        if(m_ip.empty()){
            return false;
        }
        if(m_port<0||m_port>65536){
            return false;
        }
        if(inet_addr(m_ip.c_str())==INADDR_NONE){
            return false;
        }
        return true;
    }

}