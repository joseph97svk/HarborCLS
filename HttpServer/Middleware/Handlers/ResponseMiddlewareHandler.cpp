//
// Created by josephvalverde on 12/11/23.
//

#include "ResponseMiddlewareHandler.hpp"

#include "../../Http/HttpMessages/HttpMappings.hpp"

ResponseMiddlewareHandler::ResponseMiddlewareHandler(
        Queue<std::shared_ptr<HttpResponse>>* consumingQueue,
        std::shared_ptr<HttpResponse> stopCondition) :
        Handler(consumingQueue, std::move(stopCondition)) {}

void ResponseMiddlewareHandler::optionalToEnd() {

}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

void ResponseMiddlewareHandler::handleSingle(std::shared_ptr<HttpResponse> handlingData) {
  std::vector<char> responseVector;

  std::string header;

  header += handlingData->htmlVersion + " ";
  header += HttpResponseStatusCode::getStatusCodeString(handlingData->statusCode) + "\r\n";

  if (contentTypeMap.contains(handlingData->contentType)) {
    header += "Content-Type: " + contentTypeMap.at(handlingData->contentType) + "\r\n";
  }

  header += "Content-Length: " + std::to_string(handlingData->contentLength) + "\r\n";

  for (auto& field : handlingData->otherHeaderFields) {
    header += field.fieldName + ": " + field.value + "\r\n";
  }

  header += HttpMappings::separator;
  responseVector.insert(responseVector.end(), header.begin(), header.end());

  std::visit(overloaded {
    [&responseVector](std::string& str) {
      responseVector.insert(responseVector.end(), str.begin(), str.end());
    },
    [&responseVector](std::vector<char>& vec) {
      responseVector.insert(responseVector.end(), vec.begin(), vec.end());
    }
  }, handlingData->body);

  auto socket = handlingData->socket;

  *socket << responseVector;
};
