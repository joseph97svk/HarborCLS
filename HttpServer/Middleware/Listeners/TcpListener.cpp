//
// Created by josephvalverde on 12/11/23.
//

#include <utility>

#include "TcpListener.ixx"

TcpListener::TcpListener(
    Queue<std::shared_ptr<Socket>>* connectionsQueue
    , std::shared_ptr<Socket> socket
    , ListenerMessageBundle messages
    , std::shared_ptr<Socket> stopCondition
    , std::shared_ptr<Socket> handlerStopCondition
    , unsigned int port
) : Listener(
    connectionsQueue
    , std::move(socket)
    , messages
    , std::move(stopCondition)
    , std::move(handlerStopCondition)
    , port
) {}

std::shared_ptr<Socket> TcpListener::obtain() {
  return {};
}
