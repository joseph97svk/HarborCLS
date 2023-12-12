//
// Created by josephvalverde on 12/11/23.
//

#include "RequestMiddlewareHandler.ixx"

RequestMiddlewareHandler::RequestMiddlewareHandler(
        Queue<std::shared_ptr<Socket>>* consumingQueue,
        Queue<std::shared_ptr<HttpRequest>>& producingQueue,
        std::shared_ptr<Socket> stopCondition) :
        Handler(consumingQueue, std::move(stopCondition)),
        requestsQueue(producingQueue)
        {}

void RequestMiddlewareHandler::optionalToEnd() {
}

void RequestMiddlewareHandler::handleSingle(std::shared_ptr<Socket> handlingData) {

};