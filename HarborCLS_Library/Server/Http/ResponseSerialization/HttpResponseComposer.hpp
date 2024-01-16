//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_HTTPRESPONSECOMPOSER_HPP
#define HARBOR_CLS_HTTPRESPONSECOMPOSER_HPP

#include "Server/Protocols/IResponseComposer.hpp"
#include "Server/Http/HttpMessages/HttpMappings.hpp"

namespace HarborCLS {

  class HttpResponseComposer : public IResponseComposer<HttpResponse> {
    template<class... Ts>
    struct overloaded : Ts ... {
      using Ts::operator()...;
    };

  public:
    [[nodiscard]] std::vector<char> compose(HttpResponse& response) override {
      if (response.socket == nullptr) {
        throw std::runtime_error("Dumb mistake: someone forgot to set the socket in a service running on the"
                                 " application middleware handler.");
      }

      std::vector<char> responseBody = composeBody(response);

      std::string header = generateHeader(response, responseBody.size());

      std::vector<char> responseVector = std::vector<char>(header.begin(), header.end());

      responseVector.insert(responseVector.end(), responseBody.begin(), responseBody.end());

      return responseVector;
    }

  private:
    [[nodiscard]] static std::string generateHeader(HttpResponse& response, size_t bodySize) {
      std::string header;

      header += response.htmlVersion + " ";
      header += HttpResponseStatusCode::getStatusCodeString(response.statusCode) + "\r\n";

      if (contentTypeMap.contains(response.contentType)) {
        header += "Content-Type: " + contentTypeMap.at(response.contentType) + "\r\n";
      }

      size_t contentLength = response.contentLength
          ? response.contentLength.value()
          : bodySize;

      header += "Content-Length: " + std::to_string(contentLength) + "\r\n";

      for (auto &field: response.otherHeaderFields) {
        header += field.fieldName + ": " + field.value + "\r\n";
      }

      if (!response.contentTypeAdditionalInfo.empty()) {
        header += response.contentTypeAdditionalInfo + "\r\n";
      }

      header += HttpMappings::separator;

      return header;
    }


    [[nodiscard]] static std::vector<char> composeBody(HttpResponse& response) {
      std::vector<char> body;

      std::visit(overloaded{
          [&body](std::string &str) {
            body = std::vector<char>(str.begin(), str.end());
          },
          [&body](std::vector<char> &vec) {
            body = std::move(vec);
          }
      }, response.body);

      body.emplace_back('\0');

      return body;
    }
  };
}
#endif //HARBOR_CLS_HTTPRESPONSECOMPOSER_HPP
