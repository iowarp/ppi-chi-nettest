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

#ifndef CHI_RPC_H_
#define CHI_RPC_H_

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "chimaera_types.h"
#include "config_server.h"
#include "serialize_defn.h"

namespace chi {

/** Uniquely identify a host machine */
struct HostInfo {
  std::string hostname_; /**< Host name */
  std::string ip_addr_;  /**< Host IP address */
  NodeId node_id_;       /**< Node ID */

  HostInfo() = default;
  explicit HostInfo(const std::string &hostname, const std::string &ip_addr,
                    const NodeId &node_id)
      : hostname_(hostname), ip_addr_(ip_addr), node_id_(node_id) {}
};

/** A structure to represent RPC context. */
class RpcContext {
 public:
  int port_;                    /**< port number */
  std::string protocol_;        /**< Libfabric provider */
  std::string domain_;          /**< Libfabric domain */
  std::vector<HostInfo> hosts_; /**< Hostname and ip addr per-node */
  size_t neighborhood_size_ = 32;
  u32 node_id_;
  u32 num_threads_;
  ServerConfig *config_;

 public:
  /** Default constructor */
  RpcContext() = default;

  /** initialize host info list */
  void ServerInit(ServerConfig *config) {
    config_ = config;
    port_ = config_->rpc_.port_;
    protocol_ = config_->rpc_.protocol_;
    domain_ = config_->rpc_.domain_;
    num_threads_ = config_->rpc_.num_threads_;
    if (hosts_.size()) {
      return;
    }
    // Uses hosts produced by host_names
    std::vector<std::string> &hosts = config_->rpc_.host_names_;
    // Get all host info
    hosts_.reserve(hosts.size());
    NodeId node_id = 1;
    for (const std::string &name : hosts) {
      hosts_.emplace_back(name, _GetIpAddress(name), node_id++);
    }
    // Get id of current host
    node_id_ = _FindThisHost();
    if (node_id_ == 0 || node_id_ > (u32)hosts_.size()) {
      HELOG(kFatal, "Couldn't identify this host.");
    }
    // Reset the node ids
    if (config->local_only_) {
      HostInfo this_host = hosts_[node_id_ - 1];
      hosts_.clear();
      this_host.node_id_ = 1;
      node_id_ = 1;
      hosts_.emplace_back(this_host);
    }
  }

  /** get RPC address */
  std::string GetRpcAddress(NodeId node_id, int port) {
    if (config_->rpc_.protocol_ == "shm") {
      return "shm";
    }
    std::string result = config_->rpc_.protocol_ + "://";
    if (!config_->rpc_.domain_.empty()) {
      result += config_->rpc_.domain_ + "/";
    }
    std::string host_name = GetHostNameFromNodeId(node_id);
    result += host_name + ":" + std::to_string(port);
    return result;
  }

  /** Get RPC address for this node */
  std::string GetMyRpcAddress() { return GetRpcAddress(node_id_, port_); }

  /** get host name from node ID */
  std::string GetHostNameFromNodeId(NodeId node_id) {
    // NOTE(llogan): node_id 0 is reserved as the NULL node
    if (node_id <= 0 || node_id > (i32)hosts_.size()) {
      HELOG(kFatal,
            "Attempted to get from node {}, which is out of "
            "the range 1-{}",
            node_id, hosts_.size());
    }
    u32 index = node_id - 1;
    return hosts_[index].hostname_;
  }

  /** get host name from node ID */
  std::string GetIpAddressFromNodeId(NodeId node_id) {
    // NOTE(llogan): node_id 0 is reserved as the NULL node
    if (node_id <= 0 || node_id > (u32)hosts_.size()) {
      throw std::runtime_error(hshm::Formatter::format(
          "Attempted to get from node {}, which is out of "
          "the range 1-{}",
          node_id, hosts_.size()));
    }
    u32 index = node_id - 1;
    return hosts_[index].ip_addr_;
  }

  /** Get RPC protocol */
  std::string GetProtocol() { return config_->rpc_.protocol_; }

  /** Number of hosts */
  size_t NumHosts() { return hosts_.size(); }

 private:
  /** Get the node ID of this machine according to hostfile */
  int _FindThisHost() {
    int node_id = 1;
    for (HostInfo &host : hosts_) {
      if (_IsAddressLocal(host.ip_addr_)) {
        return node_id;
      }
      ++node_id;
    }
    HELOG(kFatal, "Could not identify this host");
    return -1;
  }

  /** Check if an IP address is local */
  bool _IsAddressLocal(const std::string &addr) {
    struct ifaddrs *ifAddrList = nullptr;
    bool found = false;

    if (getifaddrs(&ifAddrList) == -1) {
      perror("getifaddrs");
      return false;
    }

    for (struct ifaddrs *ifAddr = ifAddrList; ifAddr != nullptr;
         ifAddr = ifAddr->ifa_next) {
      if (ifAddr->ifa_addr == nullptr ||
          ifAddr->ifa_addr->sa_family != AF_INET) {
        continue;
      }

      struct sockaddr_in *sin =
          reinterpret_cast<struct sockaddr_in *>(ifAddr->ifa_addr);
      char ipAddress[INET_ADDRSTRLEN] = {0};
      inet_ntop(AF_INET, &(sin->sin_addr), ipAddress, INET_ADDRSTRLEN);

      if (addr == ipAddress) {
        found = true;
        break;
      }
    }

    freeifaddrs(ifAddrList);
    return found;
  }

  /** Get IPv4 address from the host with "host_name" */
  std::string _GetIpAddress(const std::string &host_name) {
    struct hostent hostname_info = {};
    struct hostent *hostname_result;
    int hostname_error = 0;
    char hostname_buffer[4096] = {};
#ifdef __APPLE__
    hostname_result = gethostbyname(host_name.c_str());
    in_addr **addr_list = (struct in_addr **)hostname_result->h_addr_list;
#else
    int gethostbyname_result =
        gethostbyname_r(host_name.c_str(), &hostname_info, hostname_buffer,
                        4096, &hostname_result, &hostname_error);
    if (gethostbyname_result != 0) {
      HELOG(kFatal, hstrerror(h_errno));
    }
    in_addr **addr_list = (struct in_addr **)hostname_info.h_addr_list;
#endif
    if (!addr_list[0]) {
      HELOG(kFatal, hstrerror(h_errno));
    }

    char ip_address[INET_ADDRSTRLEN] = {0};
    const char *inet_result =
        inet_ntop(AF_INET, addr_list[0], ip_address, INET_ADDRSTRLEN);
    if (!inet_result) {
      perror("inet_ntop");
      HELOG(kFatal, "inet_ntop failed");
    }
    return ip_address;
  }
};

}  // namespace chi

#endif  // CHI_RPC_H_
