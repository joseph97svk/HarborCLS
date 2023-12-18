//
// Created by josephvalverde on 12/11/23.
//

#include <utility>

#include "TcpListener.hpp"

TcpListener::TcpListener(
    Queue<std::shared_ptr<TcpSocket>>* connectionsQueue
    , std::shared_ptr<TcpSocket> socket
    , ListenerMessageBundle messages
    , std::shared_ptr<TcpSocket> stopCondition
    , std::shared_ptr<TcpSocket> handlerStopCondition
    , unsigned int port
) : Listener(
    connectionsQueue
    , std::move(socket)
    , messages
    , std::move(stopCondition)
    , std::move(handlerStopCondition)
    , port
) {}

std::shared_ptr<TcpSocket> TcpListener::obtain() {
  std::shared_ptr<TcpSocket> receivedConnection = _listeningSocket->accept();

  std::cout << "Connection accepted on listener and queueing" << std::endl;

  return receivedConnection;
}
