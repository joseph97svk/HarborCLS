//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_HTTPRESPONSEHEADERCOMPOSER_HPP
#define HARBOR_CLS_HTTPRESPONSEHEADERCOMPOSER_HPP

#include "IResponseHeaderComposer.hpp"
#include "Server/Http/HttpMessages/HttpMappings.hpp"

namespace HarborCLS {

  class HttpResponseHeaderComposer : public IResponseHeaderComposer {
    template<class... Ts>
    struct overloaded : Ts ... {
      using Ts::operator()...;
    };

  public:
    std::string composeHeader(std::shared_ptr<HttpResponse> response) override {
      if (response->socket == nullptr) {
        throw std::runtime_error("Dumb mistake: someone forgot to set the socket in a service running on the"
                                 " application middleware handler.");
      }

      std::string header;

      header += response->htmlVersion + " ";
      header += HttpResponseStatusCode::getStatusCodeString(response->statusCode) + "\r\n";

      if (contentTypeMap.contains(response->contentType)) {
        header += "Content-Type: " + contentTypeMap.at(response->contentType) + "\r\n";
      }

      size_t contentLength = response->contentLength;

      if (contentLength == 0) {
        contentLength = std::visit(overloaded{
            [](std::string &str) {
              return str.size();
            },
            [](std::vector<char> &vec) {
              return vec.size();
            }
        }, response->body);
      }

      header += "Content-Length: " + std::to_string(contentLength) + "\r\n";

      for (auto &field: response->otherHeaderFields) {
        header += field.fieldName + ": " + field.value + "\r\n";
      }

      header += HttpMappings::separator;

      return header;
    }
  };
}
#endif //HARBOR_CLS_HTTPRESPONSEHEADERCOMPOSER_HPP
