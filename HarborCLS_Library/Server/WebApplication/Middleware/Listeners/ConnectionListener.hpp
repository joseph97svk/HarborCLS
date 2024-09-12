//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_TCPLISTENER_HPP
#define HTTPSERVER_TCPLISTENER_HPP

#include "../../../Socket/TcpSocket.hpp"
#include "Server/Middleware/Listener.hpp"

namespace HarborCLS {
  template<typename SocketType>
  class ConnectionListener : public Listener<std::shared_ptr<SocketType>, SocketType> {
    using ListeningType = std::shared_ptr<SocketType>;
  public:
    /**
     * Constructor for the TcpListener class.
     * @param connectionsQueue the queue where the connections will be stored.
     * @param socket the socket that will be used to listen for connections.
     * @param stopCondition the condition that will stop the listener.
     * @param handlerStopCondition the condition that will stop the handler.
     * @param port the port where the listener will listen for connections.
     */
    ConnectionListener(MiddlewareBlockingQueue<ListeningType>& connectionsQueue
                       , std::shared_ptr<SocketType> socket
                       , unsigned int port
                       , std::shared_ptr<ILogger> logger)
            : Listener<ListeningType, SocketType>(connectionsQueue
                                                                , std::move(socket)
                                                                , port
                                                                , std::move(logger)) {}

    /**
    * Obtains a connection from the listening socket.
    * @return a connection from the listening socket.
    */
    MiddlewareMessage<ListeningType> obtain() override {
      std::expected<ListeningType, SocketError> listeningSocket = this->_listeningSocket->accept();

      if (!listeningSocket) {
        return MiddlewareMessage<ListeningType>(
            Error<MessageErrors>("Error while obtaining connection from listening socket."
                                 , MessageErrors::GENERIC_ERROR));
      }

      this->_logger->info("Connection accepted");

      ListeningType receivedConnection = std::move(listeningSocket.value());

      return MiddlewareMessage<ListeningType>(receivedConnection);
    }
  };
}

#endif //HTTPSERVER_TCPLISTENER_HPP