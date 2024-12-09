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

#ifndef CHI_SRC_CONFIG_SERVER_H_
#define CHI_SRC_CONFIG_SERVER_H_

#include "chimaera_types.h"
#include "config_server.h"

namespace chi {

using hshm::BaseConfig;

/**
 * RPC information defined in server config
 * */
struct RpcInfo {
  /** The name of a file that contains host names, 1 per line */
  std::string host_file_;
  /** The parsed hostnames from the hermes conf */
  std::vector<std::string> host_names_;
  /** The RPC protocol to be used. */
  std::string protocol_;
  /** The RPC domain name for verbs transport. */
  std::string domain_;
  /** The RPC port number. */
  int port_;
  /** Number of RPC threads */
  int num_threads_;
};

/**
 * System configuration for Hermes
 */
class ServerConfig {
 public:
  /** The RPC information */
  RpcInfo rpc_;
  size_t io_size_;
  size_t md_size_;
  size_t rep_;
  size_t sleep_;
  std::string test_;

 public:
  void ServerInit(int argc, char **argv) {
    if (argc != 11) {
      HELOG(kFatal,
            "Only got {}/10 params. "
            "Usage: <test> <hostfile> <domain> <protocol> <port> <num_threads> "
            "<io_size> <md_size> <rep> <sleep>", argc);
    }
    int opt = 1;
    // argv[1]
    test_ = argv[opt++];
    // argv[2]
    rpc_.host_file_ =
        hshm::ConfigParse::ExpandPath(argv[opt++]);
    if (rpc_.host_file_.size() > 0) {
      rpc_.host_names_ = hshm::ConfigParse::ParseHostfile(rpc_.host_file_);
    } else {
      rpc_.host_names_.emplace_back("localhost");
    }
    // argv[3]
    rpc_.domain_ = argv[opt++];
    // argv[4]
    rpc_.protocol_ = argv[opt++];
    // argv[5]
    rpc_.port_ = atoi(argv[opt++]);
    // argv[6]
    rpc_.num_threads_ = atoi(argv[opt++]);
    // argv[7]
    io_size_ = hshm::ConfigParse::ParseSize(argv[opt++]);
    // argv[8]
    md_size_ = hshm::ConfigParse::ParseSize(argv[opt++]);
    // argv[9]
    rep_ = hshm::ConfigParse::ParseSize(argv[opt++]);
    // argv[10]
    sleep_ = hshm::ConfigParse::ParseSize(argv[opt++]);
  }

  void ClientInit(int argc, char **argv) {
    if (argc != 5) {
      HELOG(kFatal,
            "Only got {}/5 params. "
            "Usage: <hostfile> <domain> <protocol> <port>",
            argc);
    }
    int opt = 1;
    // argv[1]
    rpc_.host_file_ = hshm::ConfigParse::ExpandPath(argv[opt++]);
    if (rpc_.host_file_.size() > 0) {
      rpc_.host_names_ = hshm::ConfigParse::ParseHostfile(rpc_.host_file_);
    } else {
      rpc_.host_names_.emplace_back("localhost");
    }
    // argv[2]
    rpc_.domain_ = argv[opt++];
    // argv[3]
    rpc_.protocol_ = argv[opt++];
    // argv[4]
    rpc_.port_ = atoi(argv[opt++]);
  }
};

}  // namespace chi

#endif  // CHI_SRC_CONFIG_SERVER_H_
