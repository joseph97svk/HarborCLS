//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_LEGOPRESENTSERVICE_HPP
#define HARBOR_CLS_LEGOPRESENTSERVICE_HPP

#include <memory>

#include "../../LegoFigureMakerCommon/Services/StartUpPresenceNotificationService.hpp"

#include "LegoInMemoryRepository.hpp"

class LegoPresentService {
  std::shared_ptr<StartUpPresenceNotificationService> _startUpPresenceNotificationService;

  std::shared_ptr<ILegoRepository> _legoRepository;

public:
  explicit LegoPresentService(
      std::shared_ptr<StartUpPresenceNotificationService> startUpPresenceNotificationService
      , std::shared_ptr<ILegoRepository> legoRepository);

  void broadcastPresence();

private:
  inline std::vector<char> createPresenceMessage(
      LegoMessageCode code
      , int port
      , const std::string& computerIp);
};

#endif //HARBOR_CLS_LEGOPRESENTSERVICE_HPP
