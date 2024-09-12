//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_LEGODISCOVERSERVICE_HPP
#define HARBOR_CLS_LEGODISCOVERSERVICE_HPP

#include "../../LegoFigureMakerCommon/Services/StartUpPresenceNotificationService.hpp"

class LegoDiscoverService {
  std::shared_ptr<StartUpPresenceNotificationService> _startUpPresenceNotificationService;

  static const int broadcastIpId = 15;
  static const int initialVlan = 200;
  static const int finalVlan = 207;
  static const int ipDifference = 16;

  constexpr static const std::string broadcastIpBase = "172.16.123.";

public:
  explicit LegoDiscoverService(std::shared_ptr<StartUpPresenceNotificationService> startUpPresenceNotificationService)
      : _startUpPresenceNotificationService(std::move(startUpPresenceNotificationService)) {
  }

  void broadcastPresence() {
    std::vector<char> message = LegoDiscoverService::createPresenceMessage(
        LegoMessageCode::LEGO_DISCOVER
        , INTERMEDIARY_UDP_PORT
        , HarborCLS::getComputerIp());

    std::vector<std::pair<std::string, int>> targets = LegoDiscoverService::createBroadcastTargets();

    _startUpPresenceNotificationService->broadcastPresence(targets, message);
  }

protected:
  constexpr static inline std::vector<char> createPresenceMessage(
      const LegoMessageCode code
      , const int port
      , const std::string& computerIp) {

    char formatedCode = std::to_string(code)[0];
    std::string portString = std::to_string(port);

    std::vector<char> message {
        formatedCode
        , SEPARATOR
    };

    message.insert(message.end(), computerIp.begin(), computerIp.end());
    message.push_back(':');
    message.insert(message.end(), portString.begin(), portString.end());

    return message;
  }

  constexpr static inline std::vector<std::pair<std::string, int>> createBroadcastTargets() {
    std::vector<std::pair<std::string, int>> targets;

    int offset = 0;
    for (int vlan = initialVlan; vlan < finalVlan; vlan++) {
      std::string broadcastIp =
          '-'
          + broadcastIpBase
          + std::to_string(broadcastIpId + offset);

      targets.emplace_back(broadcastIp, PIECES_UDP_PORT);

      offset += ipDifference;
    }

    // broadcast to include same pc
    targets.emplace_back("", PIECES_UDP_PORT);

    return targets;
  }
};

#endif //HARBOR_CLS_LEGODISCOVERSERVICE_HPP
