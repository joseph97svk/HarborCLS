//
// Created by josephvalverde on 12/11/23.
//

#include "ResponseMiddlewareHandler.hpp"

#include <utility>

#include "../../Http/HttpMessages/HttpMappings.hpp"

ResponseMiddlewareHandler::ResponseMiddlewareHandler(
        Queue<std::shared_ptr<HttpResponse>>* consumingQueue,
        std::shared_ptr<HttpResponse> stopCondition,
        std::shared_ptr<IResponseHeaderComposer> headerComposer)
        : Handler(consumingQueue, std::move(stopCondition))
        , _headerComposer(std::move(headerComposer)){}

void ResponseMiddlewareHandler::optionalToEnd() {

}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

void ResponseMiddlewareHandler::handleSingle(std::shared_ptr<HttpResponse> handlingData) {
  std::string header = std::move(_headerComposer->composeHeader(handlingData));

  header += HttpMappings::separator;

  std::vector<char> responseVector(header.begin(), header.end());

  std::visit(overloaded {
    [&responseVector](std::string& str) {
      responseVector.insert(responseVector.end(), str.begin(), str.end());
    },
    [&responseVector](std::vector<char>& vec) {
      responseVector.insert(responseVector.end(), vec.begin(), vec.end());
    }
  }, handlingData->body);

  *handlingData->socket << responseVector;
};
