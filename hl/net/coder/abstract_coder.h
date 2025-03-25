#ifndef HL_NET_ABSTRACT_CODER_H
#define HL_NET_ABSTRACT_CODER_H
#include<vector>
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_buffer.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_protocol.h"
namespace hl
{
    class AbstractCoder{
        public:
        //将对象转化为字节流，写入buffer
        virtual void encode(std::vector<AbstractProtocol::s_ptr>&message,TcpBuffer::s_ptr out_buffer)=0;
        //将buffer里的字节流转为message对象
        virtual void decode(std::vector<AbstractProtocol::s_ptr>&out_message,TcpBuffer::s_ptr buffer)=0;

        virtual ~AbstractCoder(){

        }
        private:
    };
} // namespace hl

#endif