//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP
#define HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP

#include <HarborCLS.hpp>

#include "../../LegoFigureMakerCommon/Middleware/BroadcastListener.hpp"

#include "../Middleware/LegoPresentHandler.hpp"

class LegoServerDiscoveryService : public HarborCLS::BaseWebAppService<HarborCLS::HttpProtocol> {
  using IncommingMessageType = std::shared_ptr<typename HarborCLS::HttpProtocol::RequestType>;
  using OutgoingMessageType = std::shared_ptr<typename HarborCLS::HttpProtocol::ResponseType>;
  using BuilderReferenceWrapper = HarborCLS::BuilderReferenceWrapper<HarborCLS::HttpProtocol>;


  HarborCLS::MiddlewareBlockingQueue<std::shared_ptr<std::vector<char>>> _connectionsQueue;

  std::shared_ptr<LegoPresentHandler> _legoPresentHandler;
  std::shared_ptr<BroadcastListener> _broadcastListener;

  HarborCLS::Builder<HarborCLS::HttpProtocol>& _dependencyResolver;

public:
  explicit LegoServerDiscoveryService(std::shared_ptr<BuilderReferenceWrapper> dependencyResolver)
      : HarborCLS::BaseWebAppService<HarborCLS::HttpProtocol>()
      , _dependencyResolver(dependencyResolver->builder) {

    std::string ip;

    auto container = _dependencyResolver.createScopedContainer();
    std::shared_ptr<RoutingService> routingMapService = container->resolve<RoutingService>();

    this->setSetUpSequence([=, this](BaseWebAppService& app) {
      _broadcastListener = std::make_shared<BroadcastListener>(
          _connectionsQueue
          , std::make_shared<HarborCLS::UDPSocket>()
          , INTERMEDIARY_UDP_PORT
          , app.getLogger()
          , ip
          , INTERMEDIARY_UDP_PORT);

      _legoPresentHandler = std::make_shared<LegoPresentHandler>(
          _connectionsQueue
          , app.getLogger()
          , routingMapService);

      _broadcastListener->start();
      _legoPresentHandler->start();
    });

    this->setTearDownSequence([this](BaseWebAppService& app) {
      (void) app;

      _broadcastListener->stop();

      _broadcastListener->waitToFinish();
      _legoPresentHandler->waitToFinish();
    });
  }

  bool canHandle(IncommingMessageType request) override {
    (void) request;

    return false;
  }

  std::optional<HarborCLS::MiddlewareMessage<OutgoingMessageType>> handleTask(IncommingMessageType request) override {
    (void) request;

    return std::nullopt;
  }
};


#endif //HARBOR_CLS_LEGOSERVERDISCOVERYSERVICE_HPP
