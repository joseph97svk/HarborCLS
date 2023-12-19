//
// Created by josephvalverde on 12/11/23.
//
#include "HttpRequestParser.hpp"
#include "HttpHeaderParser.hpp"

[[nodiscard]] inline static std::vector<std::string> splitAcceptHeader(const std::string& header) {
  std::vector<std::string> result;
  std::stringstream ss(header);
  std::string item;

  while (std::getline(ss, item, ',')) {
    result.push_back(item);
  }

  return result;
}

[[nodiscard]] inline static std::shared_ptr<std::variant<std::string, std::vector<char>>> getBody(
        std::string& bodyType,
        auto& separatorLocation,
        std::vector<char>& request) {
  std::vector<std::string> acceptTypes = splitAcceptHeader(bodyType);
  bool needsTobeVectorized = false;

  for (const std::string& type : acceptTypes) {
    if (!HttpMappings::getInstance().contentTypeRequiresVectorizedBody.contains(type)) {
      continue;
    }

    if (HttpMappings::getInstance().contentTypeRequiresVectorizedBody.at(type)) {
      needsTobeVectorized = true;
      break;
    }
  }

  std::shared_ptr<std::variant<std::string, std::vector<char>>> body =
          std::make_shared<std::variant<std::string, std::vector<char>>>();

  if (needsTobeVectorized) {
    std::vector<char> bodyVector(separatorLocation + HttpMappings::separator.size(), request.end());
    *body = bodyVector;
  } else {
    std::string bodyString(separatorLocation + HttpMappings::separator.size(), request.end());
    *body = bodyString;
  }

  return body;
}

[[nodiscard]] std::shared_ptr<HttpRequest> HttpRequestParser::createHttpRequest(std::vector<char> &request,
                                                                  std::shared_ptr<TcpSocket> socket) {
  auto separatorLocation = std::search(request.begin(), request.end(), HttpMappings::separator.begin(),
                                       HttpMappings::separator.end());

  std::string headerString(request.begin(), separatorLocation);
  HttpHeader header = HttpHeaderParser::parseHeader(headerString);

  std::string bodyType = header.fields[HttpMappings::HeaderField::contentType];

  std::shared_ptr<std::variant<std::string, std::vector<char>>> body = getBody(bodyType, separatorLocation, request);

  return std::make_shared<HttpRequest>(
          socket,
          header,
          body
  );
}

