//
// Created by josephvalverde on 12/11/23.
//

#include "ApplicationMiddlewareHandler.ixx"

ApplicationMiddlewareHandler::ApplicationMiddlewareHandler(
        Queue<std::shared_ptr<HttpRequest>>* consumingQueue,
        Queue<std::shared_ptr<HttpResponse>>& producingQueue,
        std::shared_ptr<HttpRequest> stopCondition) :
        Handler(consumingQueue, std::move(stopCondition)),
        responsesQueue(producingQueue)
        {}

void ApplicationMiddlewareHandler::optionalToEnd() {

}

void ApplicationMiddlewareHandler::handleSingle(std::shared_ptr<HttpRequest> handlingData) {

};