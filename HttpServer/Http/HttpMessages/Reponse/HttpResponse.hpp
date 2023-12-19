//
// Created by josephvalverde on 12/11/23.
//
#ifndef HTTPSERVER_HTTPRESPONSE_HPP
#define HTTPSERVER_HTTPRESPONSE_HPP

#include <variant>

#include "Http/HttpMessages/HttpContentTypes.hpp"
#include "Http/HttpMessages/HttpResponseStatusCodes.hpp"
#include "Socket/TcpSocket.hpp"

struct AdditionalField {
  std::string fieldName;
  std::string value;
};

template<typename SocketType>
struct Base_HttpResponse {
  std::shared_ptr<SocketType> socket;

  std::string htmlVersion {"HTTP/1.1"};

  StatusCode statusCode {StatusCode::OK};

  ContentType contentType {ContentType::TextHtml};

  std::string contentTypeAdditionalInfo {};

  unsigned long contentLength {0};

  std::variant<std::string, std::vector<char>> body;

  std::vector<AdditionalField> otherHeaderFields;
};

using HttpResponse = Base_HttpResponse<TcpSocket>;

#endif //HTTPSERVER_HTTPRESPONSE_HPP