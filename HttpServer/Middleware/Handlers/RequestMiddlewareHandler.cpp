//
// Created by josephvalverde on 12/11/23.
//

#include "RequestMiddlewareHandler.hpp"

RequestMiddlewareHandler::RequestMiddlewareHandler(
        Queue<std::shared_ptr<TcpSocket>>* consumingQueue,
        Queue<std::shared_ptr<HttpRequest>>& producingQueue,
        std::shared_ptr<TcpSocket> stopCondition) :
        Handler(consumingQueue, std::move(stopCondition)),
        requestsQueue(producingQueue)
        {}

void RequestMiddlewareHandler::optionalToEnd() {
}

void RequestMiddlewareHandler::handleSingle(std::shared_ptr<TcpSocket> handlingData) {

};