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

#include <string.h>
#include <yaml-cpp/yaml.h>

#include <ostream>

#include "hermes_shm/util/config_parse.h"
#include "hermes_shm/util/logging.h"
#include "test_networks/config_server.h"
#include "test_networks/config_server_default.h"

namespace chi {

/** parse RPC information from YAML config */
void ServerConfig::ParseRpcInfo(YAML::Node yaml_conf) {
  std::string suffix;
  rpc_.host_names_.clear();
  if (yaml_conf["host_file"]) {
    rpc_.host_file_ =
        hshm::ConfigParse::ExpandPath(yaml_conf["host_file"].as<std::string>());
    if (rpc_.host_file_.size() > 0) {
      rpc_.host_names_ = hshm::ConfigParse::ParseHostfile(rpc_.host_file_);
    }
  }
  if (yaml_conf["host_names"] && rpc_.host_names_.size() == 0) {
    for (YAML::Node host_name_gen : yaml_conf["host_names"]) {
      std::string host_names = host_name_gen.as<std::string>();
      hshm::ConfigParse::ParseHostNameString(host_names, rpc_.host_names_);
    }
  }
  if (yaml_conf["domain"]) {
    rpc_.domain_ = yaml_conf["domain"].as<std::string>();
  }
  if (yaml_conf["protocol"]) {
    rpc_.protocol_ = yaml_conf["protocol"].as<std::string>();
  }
  if (yaml_conf["port"]) {
    rpc_.port_ = yaml_conf["port"].as<int>();
  }
  if (yaml_conf["cpus"]) {
    ClearParseVector<u32>(yaml_conf["cpus"], rpc_.cpus_);
    rpc_.num_threads_ = rpc_.cpus_.size();
  }
}

/** parse the YAML node */
void ServerConfig::ParseYAML(YAML::Node &yaml_conf) {
  if (yaml_conf["rpc"]) {
    ParseRpcInfo(yaml_conf["rpc"]);
  }
}

/** Load the default configuration */
void ServerConfig::LoadDefault() {
  LoadText(kChiServerDefaultConfigStr, false);
}

}  // namespace chi::config
