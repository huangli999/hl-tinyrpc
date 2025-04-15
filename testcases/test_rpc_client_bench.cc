#include <atomic>
#include <chrono>
#include <vector>
#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>
#include<mutex>
#include <unistd.h>
#include <google/protobuf/service.h>
#include "hl/common/log.h"
#include "hl/common/config.h"
#include "hl/common/mutex.h"
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
#include<thread>
#include "order.pb.h"
using namespace std::chrono;

int total_requests = 10000;
int thread_num = 10;
std::atomic<int> success_count{0};
std::atomic<int> fail_count{0};
std::vector<long long> latencies;

std::mutex latency_mutex;

void pressure_test_rpc(int per_thread_count) {
  for (int i = 0; i < per_thread_count; ++i) {
    auto start = high_resolution_clock::now();

    NEWRPCCHANNEL(channel, "127.0.0.1:12346");
    NEWMESSAGE(makeOrderRequest, request);
    NEWMESSAGE(makeOrderResponse, response);
    NEWRPCCONTROLLER(controller);

    controller->SetMsgId(std::to_string(rand()));

    request->set_price(100);
    request->set_goods("apple");

    std::shared_ptr<hl::RpcClosure> closure = std::make_shared<hl::RpcClosure>(
      [start, request, response, channel, controller]() mutable {

        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();  // us

        {
          std::lock_guard<std::mutex> lock(latency_mutex);
          latencies.push_back(duration);
        }

        if (controller->GetErrorCode() == 0) {
          success_count++;
        } else {
          fail_count++;
        }

        channel.reset(); // 释放连接
      }
    );

    CALLRPC("127.0.0.1:12346", Order_Stub, controller, request, response, closure, makeOrder);
    
    // 注意：此处若是异步调用，需要使用 eventloop/协程等继续跑，不然就 exit 了
    // 简单起见你可以考虑将 RpcChannel 做同步封装（或者 closure 内加条件变量+wait/notify）
  }
}

int main() {
  hl::Config::SetGlobalConfig(NULL);
  hl::Logger::InitGlobalLogger(0);

  std::vector<std::thread> threads;
  int per_thread_count = total_requests / thread_num;

  auto test_start = high_resolution_clock::now();

  for (int i = 0; i < thread_num; ++i) {
    threads.emplace_back(pressure_test_rpc, per_thread_count);
  }

  for (auto& t : threads) {
    t.join();
  }

  auto test_end = high_resolution_clock::now();
  auto total_duration = duration_cast<milliseconds>(test_end - test_start).count();

  // 统计
  long long total_latency = 0;
  long long max_latency = 0;
  for (auto t : latencies) {
    total_latency += t;
    max_latency = std::max(max_latency, t);
  }

  std::cout << "========== 压测结果 ==========" << std::endl;
  std::cout << "总请求数: " << total_requests << std::endl;
  std::cout << "成功数: " << success_count << std::endl;
  std::cout << "失败数: " << fail_count << std::endl;
  std::cout << "总耗时: " << total_duration << " ms" << std::endl;
  std::cout << "平均延迟: " << total_latency / latencies.size() << " us" << std::endl;
  std::cout << "最大延迟: " << max_latency << " us" << std::endl;
  std::cout << "QPS: " << (total_requests * 1000.0 / total_duration) << std::endl;

  return 0;
}
