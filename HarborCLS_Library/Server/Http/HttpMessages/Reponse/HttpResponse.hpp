//
// Created by josephvalverde on 12/11/23.
//
#ifndef HTTPSERVER_HTTPRESPONSE_HPP
#define HTTPSERVER_HTTPRESPONSE_HPP

#include <variant>

#include "../HttpContentTypes.hpp"
#include "../HttpResponseStatusCodes.hpp"
#include "../../../Socket/TcpSocket.hpp"

namespace HarborCLS {

  struct AdditionalField {
    std::string fieldName;
    std::string value;
  };

  template<typename SocketType>
  struct Base_HttpResponse {
    std::shared_ptr<SocketType> socket;

    std::string htmlVersion{"HTTP/1.1"};

    StatusCode statusCode{StatusCode::OK};

    ContentType contentType{ContentType::TextHtml};

    std::string contentTypeAdditionalInfo{};

    std::optional<unsigned long> contentLength {std::nullopt};

    std::variant<std::string, std::vector<char>> body;

    std::vector<AdditionalField> otherHeaderFields;
  };

  using HttpResponse = Base_HttpResponse<TcpSocket>;
}

#endif //HTTPSERVER_HTTPRESPONSE_HPP