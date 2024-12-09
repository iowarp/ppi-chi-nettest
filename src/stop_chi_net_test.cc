//
// Created by llogan on 18/11/24.
//

#include <future>
#include <thread>

#include "hermes_shm/util/config_parse.h"
#include "test_networks/rpc_thallium.h"

int main(int argc, char **argv) {
  chi::ServerConfig info;
  info.ClientInit(argc, argv);
  CHI_RPC->ServerInit(&info);
  try {
    CHI_THALLIUM->ClientInit(CHI_RPC);
  } catch (...) {
    if (CHI_RPC->node_id_ == 0) {
      HILOG(kFatal, "Failed to initialize server");
    } else {
      exit(1);
    }
  }
  CHI_THALLIUM->StopAllDaemons();
}