/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Distributed under BSD 3-Clause license.                                   *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Illinois Institute of Technology.                        *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of Hermes. The full Hermes copyright notice, including  *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the top directory. If you do not  *
 * have access to the file, you may request a copy from help@hdfgroup.org.   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CHIMAERA_RPC_THALLIUM_H_
#define CHIMAERA_RPC_THALLIUM_H_

#include <thallium.hpp>
#include <string>
#include "hermes_shm/util/singleton.h"
#include "hermes_shm/util/logging.h"
#include "chimaera_types.h"

#include "rpc.h"

namespace tl = thallium;

namespace chi {

/**
   A structure to represent Thallium state
*/
class ThalliumRpc {
 public:
  std::atomic<bool> kill_requested_; /**< is kill requested? */
  std::unique_ptr<tl::engine> client_engine_; /**< pointer to client engine */
  std::unique_ptr<tl::engine> server_engine_; /**< pointer to server engine */
  RpcContext *rpc_;
  std::vector<tl::managed<tl::xstream>> rpc_xstreams_; /**< RPC streams */
  tl::managed<tl::pool> rpc_pool_; /**< RPC pool */

  /** initialize RPC context  */
  ThalliumRpc() {}

  /** Initialize server */
  void ServerInit(RpcContext *rpc) {
    rpc_ = rpc;
    std::string addr = rpc->GetMyRpcAddress();
    try {
      server_engine_ = std::make_unique<tl::engine>(
          addr, THALLIUM_SERVER_MODE);
    } catch (std::exception &e) {
      HELOG(kFatal, "RPC init failed for host: {}\n{}", addr, e.what());
    }
    std::string rpc_server_name = server_engine_->self();
    HILOG(kInfo, "(node {}) Serving {} (i.e., {}) with {} RPC threads",
          rpc->node_id_,
          rpc_server_name,
          addr,
          rpc->num_threads_);
    // Create RPC worker threads
    rpc_pool_ = tl::pool::create(tl::pool::access::mpmc);
    for (int i = 0; i < rpc_->num_threads_; ++i) {
      tl::managed<tl::xstream> es =
          tl::xstream::create(tl::scheduler::predef::deflt, *rpc_pool_); 
      rpc_xstreams_.push_back(std::move(es));
    }
    HILOG(kInfo, "(node {}) Daemon has started", rpc_->node_id_);
    ClientInit(rpc);
  }

  /** Initialize client */
  void ClientInit(RpcContext *rpc) {
    rpc_ = rpc;
    std::string protocol = rpc->GetProtocol();
    client_engine_ = std::make_unique<tl::engine>(protocol,
                                                  THALLIUM_CLIENT_MODE,
                                                  true, 1);
//    HILOG(kInfo, "This client is on node {} (i.e., {}, proto: {})",
//          rpc->node_id_, rpc->GetHostNameFromNodeId(rpc->node_id_), protocol);
  }

  /** Run the daemon */
  void RunDaemon() {
    server_engine_->wait_for_finalize();
    HILOG(kInfo, "(node {}) Daemon has stopped", rpc_->node_id_);
  }

  /** Stop this daemon */
  void StopThisDaemon() {
    client_engine_->finalize();
    server_engine_->finalize();
  }

  /** Stop daemon rpc */
  void RpcStopDaemon(const tl::request &req) {
    HILOG(kInfo, "(node {})", rpc_->node_id_);
    req.respond(0);
    try {
      StopThisDaemon();
    } catch (...) {
      HELOG(kError, "Failed to stop daemon");
    }
  }

  /** Stop all daemons */
  void StopAllDaemons(NodeId last_node = 1) {
    std::vector<thallium::async_response> responses;
    responses.reserve(rpc_->NumHosts());
    for (NodeId node_id = 1; node_id <= rpc_->NumHosts(); ++node_id) {
      if (node_id == last_node) {
        continue;
      }
      auto ret = AsyncCall(node_id, "RpcStopDaemon");
      responses.emplace_back(std::move(ret));
    }
    for (thallium::async_response &resp : responses) {
      Wait<int>(resp);
    }
    StopThisDaemon();
  }

  /** Thallium-compatible server name */
  std::string GetServerName(NodeId node_id) {
    std::string ip_address = rpc_->GetIpAddressFromNodeId(node_id);
    return rpc_->protocol_ + "://" +
        std::string(ip_address) +
        ":" + std::to_string(rpc_->port_);
  }

  /** Register an RPC with thallium */
  template<typename RpcLambda>
  void RegisterRpc(tl::pool &pool, const char *name, RpcLambda &&lambda) {
    server_engine_->define(name, std::forward<RpcLambda>(lambda), 0, pool);
  }

  /** RPC call */
  template <typename RetT, bool ASYNC, typename... Args>
  RetT Call(NodeId node_id, const std::string &func_name, Args&&... args) {
    HILOG(kDebug, "Calling {} {} -> {}", func_name, rpc_->node_id_, node_id)
    std::string server_name = GetServerName(node_id);
    tl::remote_procedure remote_proc = client_engine_->define(func_name);
    tl::endpoint server = client_engine_->lookup(server_name);
    if constexpr(!ASYNC) {
      if constexpr (std::is_same<RetT, void>::value) {
        remote_proc.disable_response();
        remote_proc.on(server)(std::forward<Args>(args)...);
      } else {
        RetT result = remote_proc.on(server)(std::forward<Args>(args)...);
        return result;
      }
    } else {
      return remote_proc.on(server).async(std::forward<Args>(args)...);
    }
  }

  /** RPC call */
  template <typename RetT, typename... Args>
  RetT SyncCall(NodeId node_id, const std::string &func_name, Args&&... args) {
    return Call<RetT, false>(
        node_id, func_name, std::forward<Args>(args)...);
  }

  /** Async RPC call */
  template <typename... Args>
  thallium::async_response AsyncCall(NodeId node_id,
                                     const std::string &func_name,
                                     Args&&... args) {
    return Call<thallium::async_response, true>(
        node_id, func_name, std::forward<Args>(args)...);
  }

  /** Make a generic bulk */
  tl::bulk MakeBulk(
      tl::engine &engine,
      const std::vector<DataTransfer> &xfers,
      u32 chi_flag,
      tl::bulk_mode tl_flag,
      size_t &size_bytes) {
    size_bytes = 0;
    std::vector<std::pair<void *, size_t>> segments;
    segments.reserve(xfers.size());
    for (const DataTransfer &xfer : xfers) {
      if (xfer.flags_.Any(chi_flag)) {
        segments.emplace_back(xfer.data_, xfer.data_size_);
        size_bytes += xfer.data_size_;
      }
    }
    tl::bulk bulk;
    if (size_bytes) {
      bulk = engine.expose(segments, tl_flag);
    }
    return bulk;
  }

  /** Make bulk object for data transfer client-side */
  tl::bulk MakeBulkClient(const std::vector<DataTransfer> &xfer,
                          u32 chi_flag,
                          size_t &size_bytes) {
    tl::bulk_mode tl_flag;
    if (chi_flag & DT_WRITE) {
      tl_flag = tl::bulk_mode::read_only;
    } else {
      tl_flag = tl::bulk_mode::write_only;
    }
    return MakeBulk(*client_engine_, xfer, chi_flag, tl_flag, size_bytes);
  }

  /** Make bulk object for data transfer at server-side */
  tl::bulk MakeBulkServer(const  std::vector<DataTransfer> &xfer,
                          u32 chi_flag,
                          size_t &size_bytes) {
    tl::bulk_mode tl_flag;
    if (chi_flag & DT_WRITE) {
      tl_flag = tl::bulk_mode::write_only;
    } else {
      tl_flag = tl::bulk_mode::read_only;
    }
    return MakeBulk(*server_engine_, xfer, chi_flag, tl_flag, size_bytes);
  }

  /** I/O transfers */
  template<typename RetT, bool ASYNC, typename ...Args>
  RetT IoCall(i32 node_id, const std::string &func_name,
              const SegmentedTransfer &xfer, u32 io_flag, Args&& ...args) {
    HILOG(kDebug, "Calling {} {} -> {}", func_name, rpc_->node_id_, node_id)
    std::string server_name = GetServerName(node_id);
    tl::bulk bulk;
    size_t planned_bytes;
    bulk = MakeBulkClient(
        xfer.bulk_, io_flag, planned_bytes);
    tl::remote_procedure remote_proc =
        client_engine_->define(func_name);
    tl::endpoint server = client_engine_->lookup(server_name);
    if constexpr (!ASYNC) {
      if constexpr (std::is_same_v<RetT, void>) {
        remote_proc.disable_response();
        remote_proc.on(server)(bulk, xfer,
            std::forward<Args>(args)...);
      } else {
        return remote_proc.on(server)(bulk, xfer,
            std::forward<Args>(args)...);
      }
    } else {
      return remote_proc.on(server).async(bulk, xfer,
            std::forward<Args>(args)...);
    }
  }

  /** Synchronous I/O transfer */
  template<typename RetT, typename ...Args>
  RetT SyncIoCall(i32 node_id, const std::string &func_name,
                  SegmentedTransfer &xfer, u32 io_flag, Args&& ...args) {
    return IoCall<RetT, false>(
        node_id, func_name, xfer, io_flag, std::forward<Args>(args)...);
  }

  /** I/O transfers */
  template<typename ...Args>
  thallium::async_response AsyncIoCall(NodeId node_id, const char *func_name,
                                       SegmentedTransfer &xfer, u32 io_flag,
                                       Args&& ...args) {
    return IoCall<thallium::async_response, true>(
        node_id, func_name, xfer, io_flag, std::forward<Args>(args)...);
  }

  /** Handle DT_SENDER_WRITE at the server */
  size_t IoCallServerWrite(const tl::request &req,
                           tl::bulk &bulk,
                           const SegmentedTransfer &xfer) {
    tl::endpoint endpoint = req.get_endpoint();
    size_t planned_bytes, real_bytes = 0;
    tl::bulk local_wbulk =
        MakeBulkServer(xfer.bulk_, DT_WRITE,
                       planned_bytes);
    if (planned_bytes == 0) {
      return 0;
    }
    try {
      real_bytes += bulk.on(endpoint) >> local_wbulk;
    } catch (std::exception &e) {
      HELOG(kFatal, "(node {}) Failed to perform bulk I/O thallium: {}",
            rpc_->node_id_, e.what());
    }
    return real_bytes;
  }

  /** Check if request is complete */
  bool IsDone(thallium::async_response &req) {
    return req.received();
  }

  /** Wait for async thallium to complete */
  template<typename RetT>
  RetT Wait(thallium::async_response &req) {
    if constexpr(std::is_same_v<void, RetT>) {
      req.wait().as<RetT>();
    } else {
      return req.wait();
    }
  }

  /** Check if async thallium complete */
  bool IsComplete(thallium::async_response &req) {
    return req.received();
  }
};

#define CHI_THALLIUM hshm::EasySingleton<chi::ThalliumRpc>::GetInstance()
#define CHI_RPC hshm::EasySingleton<chi::RpcContext>::GetInstance()

}  // namespace hermes

#endif  // CHIMAERA_RPC_THALLIUM_H_
