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

#include "order.pb.h"

class OrderImpl : public Order {
 public:
  void makeOrder(google::protobuf::RpcController* controller,
                      const ::makeOrderRequest* request,
                      ::makeOrderResponse* response,
                      ::google::protobuf::Closure* done) {
    DEBUGLOG("start sleep 5s",NULL);
    // sleep(5);
    DEBUGLOG("end sleep 5s",NULL);
    if (request->price() < 10) {
      response->set_ret_code(-1);
      response->set_res_info("short balance");
      return;
    }
    response->set_order_id("20230514");
    // APPDEBUGLOG("call makeOrder success");
    if (done) {
      done->Run();
      delete done;
      done = NULL;
    }
  }

};


int main() {

  // if (argc != 2) {
  //   printf("Start test_rpc_server error, argc not 2 \n");
  //   printf("Start like this: \n");
  //   printf("./test_rpc_server ../conf/hl.xml \n");
  //   return 0;
  // }

  hl::Config::SetGlobalConfig("/home/hl/hl-tinyrpc/conf/hl.xml");
  hl::Logger::InitGlobalLogger();

  std::shared_ptr<OrderImpl> service = std::make_shared<OrderImpl>();
  hl::RpcDispatcher::GetRpcDispatcher()->registerService(service);

  hl::IPNetAddr::s_ptr addr = std::make_shared<hl::IPNetAddr>("127.0.0.1",12346);

  hl::TcpServer tcp_server(addr);

  tcp_server.start();

  return 0;
}