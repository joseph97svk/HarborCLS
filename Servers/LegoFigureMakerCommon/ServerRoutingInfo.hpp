//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_SERVERROUTINGINFO_HPP
#define HARBOR_CLS_SERVERROUTINGINFO_HPP

#include <string>

struct ServerRoutingInfo {
  std::string ip {};
  int port {};

  bool operator==(const ServerRoutingInfo& other) const {
    return ip == other.ip && port == other.port;
  }
};

namespace std {
  template<>
  struct hash<ServerRoutingInfo> {
    size_t operator()(const ServerRoutingInfo& routingInfo) const {
      return hash<string>()(routingInfo.ip) ^ hash<int>()(routingInfo.port);
    }
  };
}

#endif //HARBOR_CLS_SERVERROUTINGINFO_HPP
