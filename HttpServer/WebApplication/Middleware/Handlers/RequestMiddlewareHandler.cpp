//
// Created by josephvalverde on 12/11/23.
//

#include "RequestMiddlewareHandler.hpp"

RequestMiddlewareHandler::RequestMiddlewareHandler(
    Queue<std::shared_ptr<TcpSocket>>* consumingQueue,
    Queue<std::shared_ptr<HttpRequest>>& producingQueue,
    std::shared_ptr<TcpSocket> stopCondition,
    std::shared_ptr<IHttpRequestParser<TcpSocket>> requestParser)
    : Handler(consumingQueue, std::move(stopCondition))
    , _requestsQueue(producingQueue)
    , _requestParser(std::move(requestParser))
    {}

void RequestMiddlewareHandler::optionalToEnd() {
}

void RequestMiddlewareHandler::handleSingle(std::shared_ptr<TcpSocket> handlingData) {
  std::vector<char> request;

  *handlingData >> request;

  std::shared_ptr<HttpRequest> httpRequest = _requestParser->createHttpRequest(request, handlingData);

  _requestsQueue.push(httpRequest);
};