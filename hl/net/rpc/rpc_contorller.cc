#include"/home/hl/hl-tinyrpc/hl/net/rpc/rpc_controller.h"

/// @brief 封装客户端的rpc调用
namespace hl{


    void RpcConroller::Reset(){
        m_error_code=0;
        m_error_info="";
        m_msg_id="";
    
        m_is_failed=false;
       m_is_cancled=false;
    
       m_local_addr=nullptr;
       m_peer_addr=nullptr;
    
        m_timeout=1000;
    }


    bool RpcConroller::Failed() const{
        return m_is_failed;
    }


    std::string RpcConroller::ErrorText() const{
        return m_error_info;
    }


    void RpcConroller::StartCancel(){
        m_is_cancled=true;
    }


    void RpcConroller::SetFailed(const std::string& reason){
        m_error_info=reason;
    }

    bool RpcConroller::IsCanceled() const{
        return m_is_cancled;
    }


    void RpcConroller::NotifyOnCancel(google::protobuf::Closure* callback){
        
    }


    void RpcConroller::SetError(int32_t err_code,const std::string error_info){
        m_error_code=err_code;
        m_error_info=error_info;
        m_is_failed=true;
    }

    int32_t RpcConroller::GetErrorCode(){
        return m_error_code;
    }

    std::string RpcConroller::GetErrorInfo(){
        return m_error_info;
    }

    void RpcConroller::SetMsgId(const std::string&msg_id){
        m_msg_id=msg_id;
    }

    std::string RpcConroller::GetMsgId(){
        return m_msg_id;
    }

    void RpcConroller::SetLocalAddr(NetAddr::s_ptr addr){
        m_local_addr=addr;
    }
    
    void RpcConroller::SetPeerAddr(NetAddr::s_ptr addr){
        m_peer_addr=addr;
    }

    NetAddr::s_ptr RpcConroller::GetLocalAddr(){
        return m_local_addr;
    }

    NetAddr::s_ptr RpcConroller::GetPeerAddr(){
        return m_peer_addr;
    }

    void RpcConroller::SetTimeout(int timeout){
        m_timeout=timeout;
    }

    int RpcConroller::GetTimeout(){
        return m_timeout;
    }

}