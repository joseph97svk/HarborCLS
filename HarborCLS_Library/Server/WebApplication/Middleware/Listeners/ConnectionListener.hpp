//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_TCPLISTENER_HPP
#define HTTPSERVER_TCPLISTENER_HPP

#include "../../../Socket/TcpSocket.hpp"
#include "../../../BaseElements/Listener.hpp"

namespace HarborCLS {
  template<typename SocketType>
  class ConnectionListener : public Listener<std::shared_ptr<SocketType>, SocketType> {
  public:
    /**
     * Constructor for the TcpListener class.
     * @param connectionsQueue the queue where the connections will be stored.
     * @param socket the socket that will be used to listen for connections.
     * @param stopCondition the condition that will stop the listener.
     * @param handlerStopCondition the condition that will stop the handler.
     * @param port the port where the listener will listen for connections.
     */
    ConnectionListener(Queue<std::shared_ptr<SocketType>> *connectionsQueue
                       , std::shared_ptr<SocketType> socket
                       , std::shared_ptr<SocketType> stopCondition
                       , std::shared_ptr<SocketType> handlerStopCondition
                       , unsigned int port)
            : Listener<std::shared_ptr<SocketType>, SocketType>(connectionsQueue
                                                                , std::move(socket)
                                                                , std::move(stopCondition)
                                                                , std::move(handlerStopCondition)
                                                                , port) {}

    /**
    * Obtains a connection from the listening socket.
    * @return a connection from the listening socket.
    */
    std::shared_ptr<SocketType> obtain() override {
        std::shared_ptr<TcpSocket> receivedConnection = this->_listeningSocket->accept();

        return receivedConnection;
    }
  };
}

#endif //HTTPSERVER_TCPLISTENER_HPP