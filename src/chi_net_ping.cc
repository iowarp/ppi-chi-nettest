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
  TestRunner() {}

  void ServerInit() {
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
  template <bool ASYNC>
  void PingAll() {
    size_t count = 0;
    std::string empty;
    std::vector<thallium::async_response> responses;
    responses.reserve(CHI_RPC->NumHosts());
    for (NodeId node_id = 1; node_id <= CHI_RPC->NumHosts(); node_id++) {
      try {
        if constexpr (ASYNC) {
          auto ret = CHI_THALLIUM->AsyncCall(node_id, "RpcPing", empty);
          responses.emplace_back(std::move(ret));
        } else {
          CHI_THALLIUM->SyncCall<int>(node_id, "RpcPing", empty);
          ++count;
        }
      } catch (tl::margo_exception &err) {
        HELOG(kFatal, "Failed to ping node {}", node_id);
      }
    }
    if constexpr (ASYNC) {
      try {
        for (thallium::async_response &resp : responses) {
          CHI_THALLIUM->Wait<int>(resp);
          ++count;
        }
      } catch (tl::margo_exception &err) {
        HELOG(kFatal, "Failed to ping node");
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
  chi::TestRunner runner;
  hshm::File fd;

  if (info.server_) {
    hshm::SystemInfo::DestroySharedMemory("nettest");
    CHI_THALLIUM->ServerInit(CHI_RPC);
    runner.ServerInit();
    hshm::SystemInfo::CreateNewSharedMemory(fd, "nettest",
                                            hshm::Unit<size_t>::Kilobytes(1));
    CHI_THALLIUM->RunDaemon();
  } else if (CHI_RPC->node_id_ == 1) {
    if (!hshm::SystemInfo::OpenSharedMemory(fd, "nettest")) {
      HELOG(kFatal,
            "Server must not be started because shared memory is not "
            "created");
    }
    CHI_THALLIUM->ClientInit(CHI_RPC);
    runner.PingAll<false>();
    CHI_THALLIUM->StopAllDaemons();
  }
  HILOG(kInfo, "Test complete: {}", info.server_);
  exit(0);
}