//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_TCPLISTENER_HPP
#define HTTPSERVER_TCPLISTENER_HPP

#include "Socket/TcpSocket.hpp"
#include "BaseElements/ConnectionListener.hpp"

class TcpListener : public Listener<std::shared_ptr<TcpSocket>, TcpSocket> {
public:
  /**
   * Constructor for the TcpListener class.
   * @param connectionsQueue the queue where the connections will be stored.
   * @param socket the socket that will be used to listen for connections.
   * @param stopCondition the condition that will stop the listener.
   * @param handlerStopCondition the condition that will stop the handler.
   * @param port the port where the listener will listen for connections.
   */
  TcpListener(Queue<std::shared_ptr<TcpSocket>>* connectionsQueue
          , std::shared_ptr<TcpSocket> socket
          , std::shared_ptr<TcpSocket> stopCondition
          , std::shared_ptr<TcpSocket> handlerStopCondition
          , unsigned int port);
  /**
  * Obtains a connection from the listening socket.
  * @return a connection from the listening socket.
  */
  std::shared_ptr<TcpSocket> obtain() override;
};

#endif //HTTPSERVER_TCPLISTENER_HPP