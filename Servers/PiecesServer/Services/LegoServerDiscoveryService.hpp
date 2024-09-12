//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP
#define HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP

#include <HarborCLS.hpp>

#include "../../LegoFigureMakerCommon/Protocol/LegoFigureMakerProtocol.hpp"

#include "../../LegoFigureMakerCommon/Middleware/BroadcastListener.hpp"
#include "../Middleware/LegoDiscoverHandler.hpp"
#include "../../IntermediaryServer/Services/LegoServerDiscoveryService.hpp"

class LegoServerDiscoveryService : public HarborCLS::BaseWebAppService<LegoFigureMakerProtocol>{
  using IncomingMessageType = std::shared_ptr<typename LegoFigureMakerProtocol::RequestType>;
  using OutgoingMessageType = std::shared_ptr<typename LegoFigureMakerProtocol::ResponseType>;

  HarborCLS::MiddlewareBlockingQueue<std::shared_ptr<std::vector<char>>> _connectionsQueue {};

  std::shared_ptr<BroadcastListener> _broadcastListener {};
  std::shared_ptr<LegoDiscoverHandler> _legoDiscoverHandler {};

public:
  explicit LegoServerDiscoveryService(
      const std::shared_ptr<HarborCLS::BuilderReferenceWrapper<LegoFigureMakerProtocol>>& dependencyResolver) :
      HarborCLS::BaseWebAppService<LegoFigureMakerProtocol>(){

    auto container = dependencyResolver->builder.createScopedContainer();
    std::shared_ptr<ILegoRepository> repository = container->resolve<LegoInMemoryRepository>();

    this->setSetUpSequence([=, this](BaseWebAppService& app) {
      (void) app;
      std::string ip;

      _broadcastListener = std::make_shared<BroadcastListener>(
          _connectionsQueue
          , std::make_shared<HarborCLS::UDPSocket>()
          , PIECES_UDP_PORT
          , this->_logger
          , ip
          , PIECES_UDP_PORT);

      _legoDiscoverHandler = std::make_shared<LegoDiscoverHandler>(
          _connectionsQueue
          , this->_logger
          , repository);

      _broadcastListener->start();
      _legoDiscoverHandler->start();
    });

    this->setTearDownSequence([this](BaseWebAppService& app) {
      (void) app;

      _broadcastListener->stop();

      _broadcastListener->waitToFinish();
      _legoDiscoverHandler->waitToFinish();
    });
  }

  bool canHandle(IncomingMessageType request) override {
    (void) request;

    return false;
  }

  std::optional<HarborCLS::MiddlewareMessage<OutgoingMessageType>> handleTask(IncomingMessageType request) override {
    (void) request;

    return std::nullopt;
  }
};


#endif //HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP
