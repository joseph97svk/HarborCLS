//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_STARTUPPRESENCENOTIFICATIONSERVICE_HPP
#define HARBOR_CLS_STARTUPPRESENCENOTIFICATIONSERVICE_HPP

#include <vector>

#include <HarborCLS.hpp>
#include "../../../HarborCLS_Library/HarborCLS.hpp"

#include "../Protocol/LFMProtocolHeader.hpp"

class StartUpPresenceNotificationService {
  HarborCLS::UDPSocket _socket {};
public:
  void broadcastPresence(
      std::vector<std::pair<std::string, int>>& broadcastTargets
      , std::vector<char> message ) {

    for (std::pair<std::string, int>& target : broadcastTargets) {
      _socket[target.first, target.second] << message;
    }
  }
};

#endif //HARBOR_CLS_STARTUPPRESENCENOTIFICATIONSERVICE_HPP
