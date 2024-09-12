//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_LEGOPRESENTHANDLER_HPP
#define HARBOR_CLS_LEGOPRESENTHANDLER_HPP

#include <HarborCLS.hpp>
#include <utility>

#include "../../LegoFigureMakerCommon/Protocol/LegoFigureMakerProtocol.hpp"

#include "../Services/RoutingService.hpp"

class LegoPresentHandler : public HarborCLS::Handler<std::shared_ptr<std::vector<char>>> {
  std::shared_ptr<RoutingService> _routingMapService;

public:
  LegoPresentHandler(
      HarborCLS::MiddlewareBlockingQueue<std::shared_ptr<std::vector<char>>> &incomingQueue
      , std::shared_ptr<HarborCLS::ILogger> logger
      , std::shared_ptr<RoutingService> routingMapService)
      : HarborCLS::Handler<std::shared_ptr<std::vector<char>>>(incomingQueue, std::move(logger))
      , _routingMapService(std::move(routingMapService)) {
  }

  void handleSingle(std::shared_ptr<std::vector<char>> data) override {
    auto code { static_cast<LegoMessageCode>((*data)[0]) };

    if (code != std::to_string(LegoMessageCode::LEGO_PRESENT)[0]) {
      std::string errorMessage { "LegoPresentHandler received a message with an invalid code. Given value was: " };
      errorMessage += std::to_string((*data)[0]);
      errorMessage += "\n Message: ";
      errorMessage += std::string(data->begin(), data->end());

      this->_logger->error(errorMessage);

      return;
    }

    std::string_view message(data->begin() + 2, data->end());

    std::string ip(message.substr(0, message.find(':')));
    int port { std::stoi(std::string(message.substr(message.find(':') + 1))) };
    ServerRoutingInfo routingInfo = ServerRoutingInfo(ip, port);

    message.remove_prefix(message.find(SEPARATOR));

    while (!message.empty() && !(message.size() == 1 && message[0] == SEPARATOR)) {
      std::string_view messageSearchTarget = message.substr(1);
      size_t figureEnd = messageSearchTarget.find(SEPARATOR) == std::string::npos
          ? messageSearchTarget.size()
          : messageSearchTarget.find(SEPARATOR);

      std::string figureName(message.substr(1, figureEnd));

      message.remove_prefix(figureEnd + 1);

      _routingMapService->addRoute({figureName, routingInfo});
    }
  }

  void optionalToEnd() override {

  }
};



#endif //HARBOR_CLS_LEGOPRESENTHANDLER_HPP
