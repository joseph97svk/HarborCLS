//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_TCPLISTENER_HPP
#define HTTPSERVER_TCPLISTENER_HPP

#include "../../Socket/TcpSocket.hpp"
#include "../../BaseElements/ConnectionListener.hpp"
#include "../ListenerMessageBundle.hpp"

class TcpListener : public Listener<std::shared_ptr<TcpSocket>, TcpSocket> {
public:
    TcpListener(Queue<std::shared_ptr<TcpSocket>>* connectionsQueue
            , std::shared_ptr<TcpSocket> socket
            , ListenerMessageBundle messages
            , std::shared_ptr<TcpSocket> stopCondition
            , std::shared_ptr<TcpSocket> handlerStopCondition
            , unsigned int port);

    std::shared_ptr<TcpSocket> obtain() override;
};

#endif //HTTPSERVER_TCPLISTENER_HPP