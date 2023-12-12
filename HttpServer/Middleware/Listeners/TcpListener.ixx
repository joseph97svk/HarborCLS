//
// Created by josephvalverde on 12/11/23.
//

export module TcpListener;

#include "Listener.hpp"

export class TcpListener : public Listener<std::shared_ptr<Socket>> {
public:
    TcpListener(Queue<std::shared_ptr<Socket>>* connectionsQueue
            , std::shared_ptr<Socket> socket
            , ListenerMessageBundle messages
            , std::shared_ptr<Socket> stopCondition
            , std::shared_ptr<Socket> handlerStopCondition
            , unsigned int port);

    std::shared_ptr<Socket> obtain() override;
};
