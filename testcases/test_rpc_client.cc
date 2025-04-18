#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include <unistd.h>
#include <google/protobuf/service.h>
#include "hl/common/log.h"
#include "hl/common/config.h"
#include "hl/common/log.h"
#include "hl/net/tcp/tcp_client.h"
#include "hl/net/tcp/net_addr.h"
#include "hl/net/coder/string_coder.h"
#include "hl/net/coder/abstract_protocol.h"
#include "hl/net/coder/tinypb_coder.h"
#include "hl/net/coder/tinypb_protocol.h"
#include "hl/net/tcp/net_addr.h"
#include "hl/net/tcp/tcp_server.h"
#include "hl/net/rpc/rpc_dispatcher.h"
#include "hl/net/rpc/rpc_controller.h"
#include "hl/net/rpc/rpc_channel.h"
#include "hl/net/rpc/rpc_closure.h"

#include "order.pb.h"

void test_tcp_client() {

  hl::IPNetAddr::s_ptr addr = std::make_shared<hl::IPNetAddr>("127.0.0.1", 12346);
  hl::TcpClient client(addr);
  client.connect([addr, &client]() {
    DEBUGLOG("conenct to [%s] success", addr->toString().c_str());
    std::shared_ptr<hl::TinyPBProtocol> message = std::make_shared<hl::TinyPBProtocol>();
    message->m_msg_id = "99998888";
    message->m_pb_data = "test pb data";

    makeOrderRequest request;
    request.set_price(100);
    request.set_goods("apple");
    
    if (!request.SerializeToString(&(message->m_pb_data))) {
      ERRORLOG("serilize error",NULL);
      return;
    }

    message->m_method_name = "Order.makeOrder";

    client.writeMessage(message, [request](hl::AbstractProtocol::s_ptr msg_ptr) {
      DEBUGLOG("send message success, request[%s]", request.ShortDebugString().c_str());
    });


    client.readMessage("99998888", [](hl::AbstractProtocol::s_ptr msg_ptr) {
      std::shared_ptr<hl::TinyPBProtocol> message = std::dynamic_pointer_cast<hl::TinyPBProtocol>(msg_ptr);
      DEBUGLOG("msg_id[%s], get response %s", message->m_msg_id.c_str(), message->m_pb_data.c_str());
      makeOrderResponse response;

      if(!response.ParseFromString(message->m_pb_data)) {
        ERRORLOG("deserialize error",NULL);
        return;
      }

      DEBUGLOG("get response success, response[%s]", response.ShortDebugString().c_str());
    });
  });
}

void test_rpc_channel() {

//   NEWRPCCHANNEL("127.0.0.1:12345", channel);



//   request->set_price(100);
//   request->set_goods("apple");

//   NEWRPCCONTROLLER(controller);
//   controller->SetMsgId("99998888");
//   controller->SetTimeout(10000);

//   std::shared_ptr<hl::RpcClosure> closure = std::make_shared<hl::RpcClosure>(nullptr, [request, response, channel, controller]() mutable {
//     if (controller->GetErrorCode() == 0) {
//       INFOLOG("call rpc success, request[%s], response[%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
//       // 执行业务逻辑
//       if (response->order_id() == "xxx") {
//         // xx
//       }
//     } else {
//       ERRORLOG("call rpc failed, request[%s], error code[%d], error info[%s]", 
//         request->ShortDebugString().c_str(), 
//         controller->GetErrorCode(), 
//         controller->GetErrorInfo().c_str());
//     }
  
//     INFOLOG("now exit eventloop");
//     // channel->getTcpClient()->stop();
//     channel.reset();
//   });
  
//   {
//     std::shared_ptr<hl::RpcChannel> channel = std::make_shared<hl::RpcChannel>(hl::RpcChannel::FindAddr("127.0.0.1:12345"));
//     ;
//     channel->Init(controller, request, response, closure);
//     Order_Stub(channel.get()).makeOrder(controller.get(), request.get(), response.get(), closure.get());
//   }

  // CALLRPRC("127.0.0.1:12345", Order_Stub, makeOrder, controller, request, response, closure);

  

  // xxx
  // 协程


  NEWRPCCHANNEL(channel,"127.0.0.1:12346");
  NEWMESSAGE(makeOrderRequest, request);
  NEWMESSAGE(makeOrderResponse, response);
  // hl::IPNetAddr::s_ptr addr=std::make_shared<hl::IPNetAddr>("127.0.0.1",12346);
  // std::shared_ptr<hl::RpcChannel>channel=std::make_shared<hl::RpcChannel>(addr);

  // std::shared_ptr<makeOrderRequest>request=std::make_shared<makeOrderRequest>();
  request->set_price(100);
  request->set_goods("apple");

  // std::shared_ptr<makeOrderResponse>response=std::make_shared<makeOrderResponse>();

  // std::shared_ptr<hl::RpcConroller>controller=std::make_shared<hl::RpcConroller>();
  NEWRPCCONTROLLER(controller);
  controller->SetMsgId("99998888");

  std::shared_ptr<hl::RpcClosure>closure=std::make_shared<hl::RpcClosure>([request,response,channel,controller]()mutable{
    if(controller->GetErrorCode()==0){
      INFOLOG("call rpc success,request[%s],response[%s]",request->ShortDebugString().c_str(),response->ShortDebugString().c_str());
    }else{
      ERRORLOG("call rpc failed",NULL);
    }
    INFOLOG("now exit eventloop",NULL);
    // channel->getTcpClient()->stop();
    channel.reset();
  
  });

  CALLRPC("127.0.0.1:12346",Order_Stub,controller,request,response,closure,makeOrder);
  // channel->init(controller,request,response,closure);

  // Order_Stub stub(channel.get());

  // stub.makeOrder(controller.get(),request.get(),response.get(),closure.get());


}

int main() {

  hl::Config::SetGlobalConfig(NULL);
  hl::Logger::InitGlobalLogger(0);

  // test_tcp_client();
  test_rpc_channel();

  INFOLOG("test_rpc_channel end",NULL);

  return 0;
}