//
// Created by josephvalverde on 12/11/23.
//

#include <utility>

#include "ConnectionListener.hpp"

namespace HarborCLS {

  ConnectionListener::ConnectionListener(
      Queue<std::shared_ptr<TcpSocket>> *connectionsQueue, std::shared_ptr<TcpSocket> socket,
      std::shared_ptr<TcpSocket> stopCondition, std::shared_ptr<TcpSocket> handlerStopCondition, unsigned int port
  ) : Listener(
      connectionsQueue, std::move(socket), std::move(stopCondition), std::move(handlerStopCondition), port
  ) {}

  std::shared_ptr<TcpSocket> ConnectionListener::obtain() {
    std::shared_ptr<TcpSocket> receivedConnection = _listeningSocket->accept();

    return receivedConnection;
  }
}