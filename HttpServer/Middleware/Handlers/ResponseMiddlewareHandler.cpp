//
// Created by josephvalverde on 12/11/23.
//

#include "ResponseMiddlewareHandler.hpp"

ResponseMiddlewareHandler::ResponseMiddlewareHandler(
        Queue<std::shared_ptr<HttpResponse>>* consumingQueue,
        std::shared_ptr<HttpResponse> stopCondition) :
        Handler(consumingQueue, std::move(stopCondition)) {}

void ResponseMiddlewareHandler::optionalToEnd() {

}

void ResponseMiddlewareHandler::handleSingle(std::shared_ptr<HttpResponse> handlingData) {

};
