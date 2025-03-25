#ifndef HL_NET_CODER_TINY_PROTOCOL_H
#define HL_NET_CODER_TINY_PROTOCOL_H
#include"/home/hl/hl-tinyrpc/hl/net/coder/abstract_protocol.h"



namespace hl{
struct TinyPBProtocol: public AbstractProtocol{

public:

    static char PB_START;
    static char PB_END;

    


public:

    int32_t m_pk_len{0};
    int32_t m_req_id_len{0};
    //req_id 继承父类
    int32_t m_method_len{0};
    std::string m_method_name;
    int32_t m_err_code{0};
    int32_t m_err_info_len{0};
    std::string m_err_info;
    std::string m_pb_data;
    int32_t m_check_sum{0};

};
char TinyPBProtocol::PB_START=0x02;
char TinyPBProtocol::PB_END=0x03;


}



#endif
