
#include <hermes_shm/util/config_parse.h>
#include <rdma/fabric.h>
#include <rdma/fi_cm.h>
#include <rdma/fi_domain.h>
#include <rdma/fi_endpoint.h>
#include <rdma/fi_eq.h>
#include <rdma/fi_errno.h>
#include <rdma/fi_tagged.h>

#include <cstdio>
#include <iostream>

int PrintInfo(const std::vector<std::string> &hosts, const std::string &port) {
  fi_info *hints = nullptr;
  fi_info *info = nullptr;
  int ret;

  // Initialize hints
  hints = fi_allocinfo();
  if (!hints) {
    fprintf(stderr, "Failed to allocate hints\n");
    return 1;
  }

  // Query all available providers
  ret = fi_getinfo(FI_VERSION(1, 14), nullptr, nullptr, 0, hints, &info);
  if (ret) {
    fprintf(stderr, "fi_getinfo failed: %s\n", fi_strerror(-ret));
    fi_freeinfo(hints);
    return 1;
  }

  // Iterate through all providers
  fi_info *curr = info;
  while (curr) {
    std::string prov_name = curr->fabric_attr->prov_name;
    std::string domain_name = curr->domain_attr->name;
    std::string fabric_name = curr->fabric_attr->name;
    printf(" Fabric: %s\n", fabric_name.c_str());
    printf("  Provider: %s\n", prov_name.c_str());
    printf("  Domain: %s\n", domain_name.c_str());

    for (const std::string &host : hosts) {
    }
    curr = curr->next;
  }

  // Cleanup
  fi_freeinfo(hints);
  fi_freeinfo(info);
  return 0;
}

int PingAll(const std::vector<std::string> &hosts, const std::string &domain,
            const std::string &provider, const std::string &port) {
  // Initialize hints
  fi_info *hints = fi_allocinfo();
  if (!hints) {
    fprintf(stderr, "Failed to allocate hints\n");
    return 1;
  }

  // Set up hints
  hints->fabric_attr->prov_name = strdup(provider.c_str());
  hints->domain_attr->name = strdup(domain.c_str());

  for (const std::string &host : hosts) {
    // Get fabric info for this host
    fi_info *info;
    int ret = fi_getinfo(FI_VERSION(1, 14), host.c_str(), port.c_str(), 0,
                         hints, &info);
    if (ret) {
      fprintf(stderr, "fi_getinfo failed for host %s: %s\n", host.c_str(),
              fi_strerror(-ret));
      continue;
    }

    // Create fabric
    struct fid_fabric *fabric;
    ret = fi_fabric(info->fabric_attr, &fabric, nullptr);
    if (ret) {
      fprintf(stderr, "fi_fabric failed: %s\n", fi_strerror(-ret));
      fi_freeinfo(info);
      continue;
    }

    // Create domain
    struct fid_domain *domain_id;
    ret = fi_domain(fabric, info, &domain_id, nullptr);
    if (ret) {
      fprintf(stderr, "fi_domain failed: %s\n", fi_strerror(-ret));
      fi_close(&fabric->fid);
      fi_freeinfo(info);
      continue;
    }

    // Create endpoint
    struct fid_ep *ep;
    ret = fi_endpoint(domain_id, info, &ep, nullptr);
    if (ret) {
      fprintf(stderr, "fi_endpoint failed: %s\n", fi_strerror(-ret));
      fi_close(&domain_id->fid);
      fi_close(&fabric->fid);
      fi_freeinfo(info);
      continue;
    }

    // Send data
    char send_buf[1024] = "ping";
    char recv_buf[1024];
    ret = fi_send(ep, send_buf, sizeof(send_buf), nullptr, 0, nullptr);
    if (ret) {
      fprintf(stderr, "fi_send failed: %s\n", fi_strerror(-ret));
    }

    // Receive response
    ret = fi_recv(ep, recv_buf, sizeof(recv_buf), nullptr, 0, nullptr);
    if (ret) {
      fprintf(stderr, "fi_recv failed: %s\n", fi_strerror(-ret));
    }

    // Cleanup
    fi_close(&ep->fid);
    fi_close(&domain_id->fid);
    fi_close(&fabric->fid);
    fi_freeinfo(info);
  }

  fi_freeinfo(hints);
  return 0;
}

int LaunchServer(const std::vector<std::string> &hosts,
                 const std::string &domain, const std::string &provider,
                 const std::string &port) {
  // Initialize hints
  fi_info *hints = fi_allocinfo();
  if (!hints) {
    fprintf(stderr, "Failed to allocate hints\n");
    return 1;
  }

  // Set up hints
  hints->fabric_attr->prov_name = strdup(provider.c_str());
  hints->domain_attr->name = strdup(domain.c_str());

  // Get fabric info
  fi_info *info;
  int ret = fi_getinfo(FI_VERSION(1, 14), nullptr, port.c_str(), FI_SOURCE,
                       hints, &info);
  if (ret) {
    fprintf(stderr, "fi_getinfo failed: %s\n", fi_strerror(-ret));
    fi_freeinfo(hints);
    return 1;
  }

  // Create fabric
  struct fid_fabric *fabric;
  ret = fi_fabric(info->fabric_attr, &fabric, nullptr);
  if (ret) {
    fprintf(stderr, "fi_fabric failed: %s\n", fi_strerror(-ret));
    fi_freeinfo(info);
    fi_freeinfo(hints);
    return 1;
  }

  // Create domain
  struct fid_domain *domain_id;
  ret = fi_domain(fabric, info, &domain_id, nullptr);
  if (ret) {
    fprintf(stderr, "fi_domain failed: %s\n", fi_strerror(-ret));
    fi_close(&fabric->fid);
    fi_freeinfo(info);
    fi_freeinfo(hints);
    return 1;
  }

  // Create passive endpoint
  struct fid_pep *pep;
  ret = fi_passive_ep(fabric, info, &pep, nullptr);
  if (ret) {
    fprintf(stderr, "fi_passive_ep failed: %s\n", fi_strerror(-ret));
    fi_close(&domain_id->fid);
    fi_close(&fabric->fid);
    fi_freeinfo(info);
    fi_freeinfo(hints);
    return 1;
  }

  // Listen for connections
  ret = fi_listen(pep);
  if (ret) {
    fprintf(stderr, "fi_listen failed: %s\n", fi_strerror(-ret));
    fi_close(&pep->fid);
    fi_close(&domain_id->fid);
    fi_close(&fabric->fid);
    fi_freeinfo(info);
    fi_freeinfo(hints);
    return 1;
  }

  return 0;
}

int main(int argc, char **argv) {
  // Hostfile path
  if (argc < 6) {
    fprintf(stderr, "Usage: %s <hostfile> <domain> <provider> <port> <mode>\n",
            argv[0]);
    return 1;
  }
  std::string hostfile_path = argv[1];
  std::string domain = argv[2];
  std::string provider = argv[3];
  std::string port = argv[4];
  std::string mode = argv[5];

  // Load hostfile
  std::vector<std::string> hosts;
  if (!hostfile_path.empty()) {
    hshm::ConfigParse::ParseHostfile(hostfile_path);
  }
  if (hosts.size() == 0) {
    hosts.emplace_back("localhost");
  }

  // Launch server
  if (mode == "server") {
    return LaunchServer(hosts, domain, provider, port);
  }

  // Ping all servers
  if (mode == "client") {
    return PingAll(hosts, domain, provider, port);
  }
}