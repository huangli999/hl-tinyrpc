#ifndef HL_NET_CODER_TINY_CODER_H
#define HL_NET_CODER_TINY_CODER_H
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_coder.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/tinypb_protocol.h"


namespace hl{
class TinyPBCoder: public AbstractCoder{

public:

    TinyPBCoder();
    ~TinyPBCoder();
public:

       void encode(std::vector<AbstractProtocol::s_ptr>&message,TcpBuffer::s_ptr out_buffer);

       void decode(std::vector<AbstractProtocol::s_ptr>&out_message,TcpBuffer::s_ptr buffer);

private:
    //将单个包转为字节流
    const char*encodeTinyPB(std::shared_ptr<TinyPBProtocol>msg,int &len);   

};
}



#endif
