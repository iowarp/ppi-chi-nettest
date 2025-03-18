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
      } catch (...) {
        HELOG(kFatal, "Failed to ping node");
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
  }
};

}  // namespace chi

int main(int argc, char **argv) {
  chi::ServerConfig info;
  info.PingInit(argc, argv);
  CHI_RPC->ServerInit(&info);
  chi::TestRunner runner;
  hshm::File fd;
  std::string shm_name = hshm::Formatter::format("nettest_{}", CHI_RPC->port_);
  try {
    if (info.test_ == "server" || info.test_ == "touchserver") {
      hshm::SystemInfo::DestroySharedMemory(shm_name);
      CHI_THALLIUM->ServerInit(CHI_RPC);
      runner.ServerInit();
      hshm::SystemInfo::CreateNewSharedMemory(fd, shm_name,
                                              hshm::Unit<size_t>::Kilobytes(1));
    }
    if (info.test_ == "server") {
      CHI_THALLIUM->RunDaemon();
    }
    if (info.test_ == "client" && CHI_RPC->node_id_ == 1) {
      if (!hshm::SystemInfo::OpenSharedMemory(fd, shm_name)) {
        HELOG(kFatal,
              "Server must not be started because shared memory is not "
              "created");
      }
      CHI_THALLIUM->ClientInit(CHI_RPC);
      runner.PingAll<false>();
      CHI_THALLIUM->StopAllDaemons();

      HILOG(kInfo, "Test complete: {}",
            CHI_RPC->GetRpcAddress(1, CHI_RPC->port_));
      hshm::SystemInfo::DestroySharedMemory(shm_name);
      exit(0);
    }
  } catch (...) {
    exit(1);
  }
}