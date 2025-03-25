#include"/home/hl/hl-tinyrpc/hl/net/coder/tinypb_coder.h"
#include"/home/hl/hl-tinyrpc/hl/net/coder/tinypb_protocol.h"
#include<vector>
#include"/home/hl/hl-tinyrpc/hl/common/util.h"
#include"/home/hl/hl-tinyrpc/hl/common/log.h"
#include<string.h>
namespace hl
{
    void TinyPBCoder::encode(std::vector<AbstractProtocol::s_ptr>&message,TcpBuffer::s_ptr out_buffer){

    }

    void TinyPBCoder::decode(std::vector<AbstractProtocol::s_ptr>&out_message,TcpBuffer::s_ptr buffer){
        //遍历buffer，找到PB_START，找到整包长度，校验结束符位置


    while(1){
        std::vector<char>tmp=buffer->m_buffer;
        int start_index=buffer->readIndex();
        int end_index=-1;

        int pk_len=0;
        bool parse_success=false;
        int i=0;
        for(i=start_index;i<buffer->writeIndex();++i){
            if(tmp[i]==TinyPBProtocol::PB_START){
                if(i+1<buffer->writeIndex()){
                    pk_len=getInt32FromNetByte(&tmp[i+1]);
                    DEBUGLOG("get pk_len=%d",pk_len);
                    //结束符索引
                    int j=i+pk_len-1;
                    if(j>=buffer->writeIndex()){
                        continue;
                    }
                    if(tmp[j]==TinyPBProtocol::PB_END){
                        start_index=i;
                        end_index=j;
                        parse_success=true;
                        break;
                    }
                }
            }
        }
        if(i>=buffer->writeIndex()){
            DEBUGLOG("read all data",NULL);
            return;
        }
        if(parse_success){
            buffer->moveWriteIndex(end_index-start_index+1);
            std::shared_ptr<TinyPBProtocol> message=std::make_shared<TinyPBProtocol>();
            message->m_pk_len=pk_len;
            
            int req_id_len_index=start_index+sizeof(char)+sizeof(message->m_pk_len);
            if(req_id_len_index>end_index){
                message->parse_success=false;
                ERRORLOG("parse error,req_id_len_inde[%d]>=end_index[%d]",req_id_len_index,end_index);

            }
            message->m_req_id_len=getInt32FromNetByte(&tmp[req_id_len_index]);
            DEBUGLOG("parse m_req_id_len success",NULL);

            int req_id_index=req_id_len_index+sizeof(message->m_req_id_len);
            
            char req_id[100]={0};
            memcpy(&req_id[0],&tmp[req_id_index],message->m_req_id_len);
            message->m_req_id=std::string(req_id);
            DEBUGLOG("parse req_id=%s",message->m_req_id.c_str());

            int method_name_len_index=req_id_index+message->m_req_id_len;
            if(method_name_len_index>=end_index){
                message->parse_success=false;
                ERRORLOG("parse error,method_name_len_index[%d]>=end_index[%d]",method_name_len_index,end_index);
            }
            message->m_method_name_len=getInt32FromNetByte(&tmp[method_name_len_index]);
            int method_name_index=method_name_len_index+sizeof(message->m_method_name_len);
            char method_name[512]={0};
            memcpy(&method_name[0],&tmp[method_name_index],message->m_method_name_len);
            message->m_method_name=std::string(method_name);
            DEBUGLOG("parse m_method_name=%s",message->m_method_name.c_str());

            int err_code_index=method_name_index+message->m_method_name_len;
            if(err_code_index>=end_index){
                message->parse_success=false;
                ERRORLOG("parse error,err_code_indexx[%d]>=end_index[%d]",err_code_index,end_index);
            }
            message->m_err_code=getInt32FromNetByte(&tmp[err_code_index]);

            int error_info_len_index=err_code_index+sizeof(message->m_err_code);
            if(error_info_len_index>=end_index){
                message->parse_success=false;
                ERRORLOG("parse error,error_info_len_index[%d]>=end_index[%d]",error_info_len_index,end_index);
            }
            message->m_err_info_len=getInt32FromNetByte(&tmp[error_info_len_index]);

            int err_info_index=error_info_len_index+sizeof(message->m_err_info_len);
            char errorinfo[512]={0};
            memcpy(&errorinfo[0],&tmp[err_info_index],message->m_err_info_len);
            message->m_err_info=std::string(errorinfo);
            DEBUGLOG("parse m_err_info=%s",message->m_err_info.c_str());

            
            int pb_data_len=message->m_pk_len-message->m_method_name_len-message->m_req_id_len-message->m_err_info_len-2-24;

            int pd_data_index=err_info_index+message->m_err_info_len;
            message->m_pb_data=std::string(&tmp[pd_data_index],pb_data_len);

            //校验和解析

            message->parse_success=true;
            out_message.push_back(message);
        }
    }
    }
} // namespace hl
