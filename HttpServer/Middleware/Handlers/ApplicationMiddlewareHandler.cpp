//
// Created by josephvalverde on 12/11/23.
//

#include "ApplicationMiddlewareHandler.hpp"


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
  std::cout << "Application middleware handler\n" << std::endl;

  std::shared_ptr<HttpResponse> response = std::make_shared<HttpResponse>();
  response->socket = handlingData->socket;

  std::string body = "<html><body><h1>It works!</h1></body></html>";

  response->body = body;
  response->contentLength = body.length();

  std::cout << "Application middleware handler: handing over!!!\n" << std::endl;

  responsesQueue.push(response);
};