//
// Created by josephvalverde on 1/11/24.
//

#include "LegoPresentService.hpp"

static const int broadcastIpId = 15;
static const int initialVlan = 200;
static const int finalVlan = 207;
static const int ipDifference = 16;

constexpr static const std::string broadcastIpBase = "172.16.123.";

constexpr static inline std::vector<std::pair<std::string, int>> createBroadcastTargets();

LegoPresentService:: LegoPresentService(
    std::shared_ptr<StartUpPresenceNotificationService> startUpPresenceNotificationService
    , std::shared_ptr<ILegoRepository> legoRepository)
: _startUpPresenceNotificationService(std::move(startUpPresenceNotificationService))
, _legoRepository(std::move(legoRepository)) {
}

void LegoPresentService::broadcastPresence() {
  std::vector<char> message = LegoPresentService::createPresenceMessage(
      LegoMessageCode::LEGO_PRESENT
      , PIECES_UDP_PORT
      , HarborCLS::getComputerIp());

  std::vector<std::pair<std::string, int>> targets = createBroadcastTargets();

  _startUpPresenceNotificationService->broadcastPresence(targets, message);
}

constexpr static inline std::vector<std::pair<std::string, int>> createBroadcastTargets() {
  std::vector<std::pair<std::string, int>> targets;

  int offset = 0;
  for (int vlan = initialVlan; vlan < finalVlan; vlan++) {
    std::string broadcastIp =
        '-'
        + broadcastIpBase
        + std::to_string(broadcastIpId);

    targets.emplace_back(broadcastIp, INTERMEDIARY_UDP_PORT);

    offset += ipDifference;
  }

  // broadcast to include same pc
  targets.emplace_back("", INTERMEDIARY_UDP_PORT);

  return targets;
}

inline std::vector<char> LegoPresentService::createPresenceMessage(
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
  message.push_back(SEPARATOR);

  std::vector<std::string> legoFigures = _legoRepository->getAllNames();

  for (const std::string& legoFigure : legoFigures) {
    message.insert(message.end(), legoFigure.begin(), legoFigure.end());
    message.push_back(SEPARATOR);
  }

  std::cout << "Message: " << std::endl;

  for (char c : message) {
    std::cout << c;
  }

  std::cout << std::endl;

  return message;
}