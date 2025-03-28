#ifndef HL_NET_TCP_NET_ADDR_H
#define HL_NET_TCP_NET_ADDR_H
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string>
#include<memory>
namespace hl
{
class NetAddr{

public:

typedef std::shared_ptr<NetAddr>s_ptr;//基类指针

virtual sockaddr*getSockAddr()=0;

virtual socklen_t getSockLen()=0;

virtual int getFamily()=0;

virtual std::string toString()=0;

virtual bool checkVaild()=0;

private:
};



class IPNetAddr:public NetAddr{
public:

IPNetAddr(const std::string&ip,uint16_t port);

IPNetAddr(const std::string&addr);

IPNetAddr(sockaddr_in addr);


sockaddr*getSockAddr();

socklen_t getSockLen();

int getFamily();

std::string toString();

bool checkVaild();

private:

std::string m_ip;
uint16_t m_port{0};
sockaddr_in m_addr;



};
} // namespace hl



#endif