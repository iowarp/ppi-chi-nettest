//
// Created by llogan on 18/11/24.
//

#include <future>
#include <thread>

#include "hermes_shm/util/config_parse.h"
#include "test_networks/rpc_thallium.h"

namespace chi {

class TestRunner {
 public:
  TestRunner() {
    CHI_THALLIUM->RegisterRpc(
        *CHI_THALLIUM->rpc_pool_, "RpcPing",
        [this](const tl::request &req, const std::string &md) {
          this->RpcPing(req, md);
        });
  }

  /** The RPC for processing a message with data */
  void RpcTaskSubmit(const tl::request &req, tl::bulk &bulk,
                     chi::SegmentedTransfer &xfer) {
    xfer.AllocateBulksServer();
    CHI_THALLIUM->IoCallServerWrite(req, bulk, xfer);
    req.respond(0);
  }

  /** An empty ping */
  void RpcPing(const tl::request &req, const std::string &md) {
    req.respond(0);
  }

  /** Ping every node */
  template<bool ASYNC>
  void PingAll() {
    size_t count = 0;
    std::string empty;
    std::vector<thallium::async_response> responses;
    responses.reserve(CHI_RPC->NumHosts());
    for (NodeId node_id = 1; node_id <= CHI_RPC->NumHosts(); node_id++) {
      try {
        if constexpr (ASYNC) {
          auto ret = CHI_THALLIUM->AsyncCall(node_id, "RpcPing", empty);
        } else {
          CHI_THALLIUM->SyncCall<int>(node_id, "RpcPing", empty);
          ++count;
        }
      } catch (tl::margo_exception &err) {
      }
    }
    if constexpr (ASYNC) {
      try {
        for (thallium::async_response &resp : responses) {
          CHI_THALLIUM->Wait<int>(resp);
          ++count;
        }
      } catch (tl::margo_exception &err) {
      }
    }
    HILOG(kInfo, "Pinged {} nodes", count);
  }
};

}  // namespace chi

int main(int argc, char **argv) {
  chi::ServerConfig info;
  info.PingInit(argc, argv);
  CHI_RPC->ServerInit(&info);
  try {
    CHI_THALLIUM->ServerInit(CHI_RPC);
  } catch (...) {
    if (CHI_RPC->node_id_ == 0) {
      HILOG(kFatal, "Failed to initialize server");
    } else {
      exit(1);
    }
  }

  auto daemon =
      std::async(std::launch::async, []() { CHI_THALLIUM->RunDaemon(); });
  std::this_thread::sleep_for(std::chrono::seconds(info.sleep_));
  HILOG(kInfo, "Done sleeping!");
  chi::TestRunner runner;
  if (CHI_RPC->node_id_ == 1) {
    if (info.test_ == "ping") {
      runner.PingAll<false>();
    } else if (info.test_ == "ping_async") {
      runner.PingAll<true>();
    }
    CHI_THALLIUM->StopAllDaemons();
  }
  daemon.wait();
}