#ifndef HL_NET_STRING_CODER_H
#define HL_NET_STRING_CODER_H
#include"/home/hl/hl-tinyrpc/hl/net/abstract_coder.h"
#include"/home/hl/hl-tinyrpc/hl/net/abstract_protocol.h"
namespace hl{



    class stringProtocol:public AbstractProtocol{

        public:
        std::string m_info;
        };



    class StringCoder:public AbstractCoder{


        void encode(std::vector<AbstractProtocol::s_ptr>&message,TcpBuffer::s_ptr out_buffer){
            
            for(size_t i=0;i<message.size();++i){
                std::shared_ptr<stringProtocol>msg=std::dynamic_pointer_cast<stringProtocol>(message[i]);
                out_buffer->writeToBuffer(msg->m_info.c_str(),msg->m_info.length());
            }
        }
   
        void decode(std::vector<AbstractProtocol::s_ptr>&out_message,TcpBuffer::s_ptr buffer){

            std::vector<char>re;
            buffer->readFromBuffer(re,buffer->readAble());
            std::string info;
            for(size_t i=0;i<re.size();++i){
                info+=re[i];
            }
            std::shared_ptr<stringProtocol>msg=std::make_shared<stringProtocol>();
            msg->m_info=info;
            msg->setReqId("123456789");
            out_message.push_back(msg);

        }

    };



}


#endif