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
  /** CPU bindings for workers */
  std::vector<u32> cpus_;
};

/**
 * System configuration for Hermes
 */
class ServerConfig : public BaseConfig {
 public:
  /** The RPC information */
  RpcInfo rpc_;
  /** Bootstrap task registry */
  std::vector<std::string> modules_;

 public:
  ServerConfig() = default;
  void LoadDefault();

 private:
  void ParseYAML(YAML::Node &yaml_conf);
  void ParseWorkOrchestrator(YAML::Node yaml_conf);
  void ParseQueueManager(YAML::Node yaml_conf);
  void ParseRpcInfo(YAML::Node yaml_conf);
};

}  // namespace chi

#endif  // CHI_SRC_CONFIG_SERVER_H_
