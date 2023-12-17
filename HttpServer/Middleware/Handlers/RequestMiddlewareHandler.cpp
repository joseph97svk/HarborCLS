//
// Created by josephvalverde on 12/11/23.
//

#include "RequestMiddlewareHandler.hpp"

#include "../../Http/IHttpRequestParser.hpp"
#include "../../Http/HttpRequestParser.hpp"
#include "../../Http/HttpMessages/HttpRequest.hpp"
#include "../../Http/HttpMessages/HttpMappings.hpp"

RequestMiddlewareHandler::RequestMiddlewareHandler(
    Queue<std::shared_ptr<TcpSocket>>* consumingQueue,
    Queue<std::shared_ptr<HttpRequest>>& producingQueue,
    std::shared_ptr<TcpSocket> stopCondition) :
    Handler(consumingQueue, std::move(stopCondition)),
    _requestsQueue(producingQueue)
    {}

void RequestMiddlewareHandler::optionalToEnd() {
}

void RequestMiddlewareHandler::handleSingle(std::shared_ptr<TcpSocket> handlingData) {
  std::unique_ptr<IHttpRequestParser<TcpSocket>> parser = std::make_unique<HttpRequestParser>();

  std::vector<char> request;

  *handlingData >> request;

  std::shared_ptr<HttpRequest> httpRequest = parser->createHttpRequest(request, handlingData);

  _requestsQueue.push(httpRequest);
};