//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP
#define HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP

#include <HarborCLS.hpp>

#include "../../LegoFigureMakerCommon/Protocol/LegoFigureMakerProtocol.hpp"

#include "../../LegoFigureMakerCommon/Middleware/BroadcastListener.hpp"
#include "../Middleware/LegoDiscoverHandler.hpp"

class LegoServerDiscoveryService : HarborCLS::BaseWebAppService<LegoFigureMakerProtocol>{
  HarborCLS::MiddlewareBlockingQueue<std::shared_ptr<std::vector<char>>> _connectionsQueue;

  std::shared_ptr<BroadcastListener> _broadcastListener;

public:
  LegoServerDiscoveryService() {
    std::string ip;
    _broadcastListener = std::make_shared<BroadcastListener>(
        _connectionsQueue
        , std::make_shared<HarborCLS::UDPSocket>()
        , 0
        , this->_logger
        , ip
        , 0);

    this->setSetUpSequence([this](BaseWebAppService& app) {
      _broadcastListener->start();
    });

    this->setTearDownSequence([this](BaseWebAppService& app) {
      _broadcastListener->stop();
    });
  }
};


#endif //HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP
