#include <memory>
#include "hl/common/log.h"
#include "hl/net/tcp/net_addr.h"
#include"/home/hl/hl-tinyrpc/hl/common/config.h"
#include"/home/hl/hl-tinyrpc/hl/net/tcp/tcp_server.h"
void test_tcp_server() {

  hl::IPNetAddr::s_ptr addr = std::make_shared<hl::IPNetAddr>("127.0.0.1", 12346);

  DEBUGLOG("create addr %s", addr->toString().c_str());

  hl::TcpServer tcp_server(addr);

  tcp_server.start();

}

int main() {

  // hl::Config::SetGlobalConfig("../conf/hl.xml");
  // hl::Logger::InitGlobalLogger();

  hl::Config::SetGlobalConfig("/home/hl/hl-tinyrpc/conf/hl.xml");
  hl::Logger::InitGlobalLogger();
  test_tcp_server();
}