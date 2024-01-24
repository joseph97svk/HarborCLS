//
// Created by josephvalverde on 1/15/24.
//

#ifndef HARBOR_CLS_FIGURESSERVICE_HPP
#define HARBOR_CLS_FIGURESSERVICE_HPP

#include <memory>
#include <vector>
#include <iostream>

#include "RoutingService.hpp"

class FiguresService {
  std::shared_ptr<RoutingService> _routingMapService;

  enum class ConnectionResult {
    CONNECTION_SUCCESSFUL,
    CONNECTION_REFUSED,
    CONNECTION_TIMED_OUT,
  };

public:
  explicit FiguresService(std::shared_ptr<RoutingService> routingMapService)
      : _routingMapService(std::move(routingMapService)) {
  }

  std::vector<std::string> getFigures() {
    return _routingMapService->getAllFigures();
  }

  void print() {
    std::cout << _routingMapService->toString() << std::endl;
  }

  std::optional<std::string> getFigure(std::string& figureName) {
    std::optional<ServerRoutingInfo> routingInfo = _routingMapService->getRoutingInfo(figureName);
    if (!routingInfo) {
      return std::nullopt;
    }

    std::vector<char> message = createMessage(figureName);
    auto requestedFigure = sendRequest(message, *routingInfo);
    if (!requestedFigure) {
      return std::nullopt;
    }

    std::vector<char> figureTable = *requestedFigure;

    return std::string(figureTable.begin(), figureTable.end());
  }

  std::optional<std::vector<char>> getLegoImage(std::string& path) {
    std::string_view figureNameBuffer = path;
    figureNameBuffer.remove_prefix(figureNameBuffer.find_first_of('/') + 1);
    figureNameBuffer.remove_suffix(figureNameBuffer.size() - figureNameBuffer.find_first_of('/'));

    std::string figureName(figureNameBuffer);

    std::optional<ServerRoutingInfo> routingInfo = _routingMapService->getRoutingInfo(figureName);
    if (!routingInfo) {
      return std::nullopt;
    }

    std::vector<char> message = createMessage(path);
    auto receivedMessage = sendRequest(message, *routingInfo);
    if (!receivedMessage) {
      return std::nullopt;
    }

    std::vector<char> image = *receivedMessage;

    return image;
  }

private:
  bool connectToPiecesServer(HarborCLS::TcpSocket& socket, ServerRoutingInfo& routingInfo) {
    constexpr int timeouts[] = { FIRST_TIMEOUT, SECOND_TIMEOUT, THIRD_TIMEOUT };

    for (auto& timeout : timeouts) {
      switch (this->tryConnection(socket, routingInfo, timeout)) {
        case ConnectionResult::CONNECTION_SUCCESSFUL:
          return true;
        case ConnectionResult::CONNECTION_REFUSED:
          return false;
        case ConnectionResult::CONNECTION_TIMED_OUT:
          continue;
      }
    }

    return false;
  }

  ConnectionResult tryConnection(HarborCLS::TcpSocket& socket, ServerRoutingInfo& routingInfo, int timeout) {
    socket.setTimeout(0, timeout);
    auto connected = socket.connect(routingInfo.ip, routingInfo.port);
    if (connected) {
      return ConnectionResult::CONNECTION_SUCCESSFUL;
    }
    if (connected.error().errorType() == HarborCLS::SocketErrors::SOCKET_CONNECTION_REFUSED) {
      _routingMapService->erase(routingInfo);
      return ConnectionResult::CONNECTION_REFUSED;
    }

    return ConnectionResult::CONNECTION_TIMED_OUT;
  }

  static inline std::vector<char> createMessage(std::string figure) {
    char formatedCode = std::to_string(LegoMessageCode::LEGO_REQUEST)[0];
    std::vector<char> message { formatedCode, SEPARATOR };
    message.insert(message.end(), figure.begin(), figure.end());

    return message;
  }

  std::optional<std::vector<char>> sendRequest(std::vector<char>& message, ServerRoutingInfo& routingInfo) {
    HarborCLS::TcpSocket socket;

    if (!this->connectToPiecesServer(socket, routingInfo)) {
      return std::nullopt;
    }

    std::vector<char> response;

    socket.resetTimeout();
    socket << message;
    socket >> response;

    std::string codeString { response[0] };
    auto code = static_cast<LegoMessageCode>(std::stoi(codeString));

    if (code != LegoMessageCode::LEGO_RESPONSE) {
      return std::nullopt;
    }

    response = std::vector<char>(response.begin() + 2, response.end());

    std::cout << "image received with size: " << response.size() << std::endl;

    return response;
  }
};

#endif //HARBOR_CLS_FIGURESSERVICE_HPP
