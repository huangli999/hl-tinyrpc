#ifndef HL_NET_RPC_RPC_CLOSURE_H
#define HL_NET_RPC_RPC_CLOSURE_H
#include<google/protobuf/stubs/callback.h>
#include<functional>
namespace hl
{
    class RpcClosure:public google::protobuf::Closure{  
        public:
      RpcClosure(std::function<void()>cb):m_cb(cb){

      }
      ~RpcClosure(){
        
      }
        void Run() override{
            if(m_cb!=nullptr){
                m_cb();
            }
        }
        private:
        std::function<void()>m_cb{nullptr};
    };
} // namespace hl


#endif