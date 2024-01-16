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

      std::pair<std::vector<char>, bool> responseBody = composeBody(response);

      std::string header = generateHeader(response, responseBody.first.size(), responseBody.second);

      std::vector<char> responseVector = std::vector<char>(header.begin(), header.end());

      responseVector.insert(responseVector.end(), responseBody.first.begin(), responseBody.first.end());

      return responseVector;
    }

  private:
    [[nodiscard]] static std::string generateHeader(HttpResponse& response, size_t bodySize, bool vectorized) {
      std::string header;

      header += response.htmlVersion + " ";
      header += HttpResponseStatusCode::getStatusCodeString(response.statusCode) + HttpMappings::separator;

      if (contentTypeMap.contains(response.contentType)) {
        header += "Content-Type: " + contentTypeMap.at(response.contentType) + HttpMappings::separator;
      }

      for (auto &field: response.otherHeaderFields) {
        header += field.fieldName + ": " + field.value + HttpMappings::separator;
      }

      size_t contentLength = response.contentLength
                             ? response.contentLength.value()
                             : bodySize;

      header += "Content-Length: " + std::to_string(contentLength) + HttpMappings::separator;

      if (!response.contentTypeAdditionalInfo.empty()) {
        header += response.contentTypeAdditionalInfo + HttpMappings::separator;
      }

      header += HttpMappings::separator;
      if (!vectorized) {
        header += HttpMappings::separator;
      }

      return header;
    }

    [[nodiscard]] static std::pair<std::vector<char>, bool> composeBody(HttpResponse& response) {
      std::pair<std::vector<char>, bool> body {{}, false};

      std::visit(overloaded{
          [&body](std::string &str) {
            body.first = std::move(std::vector<char>(str.begin(), str.end()));
            body.first.emplace_back('\0');
          },
          [&body](std::vector<char> &vec) {
            body.first = std::move(vec);
            body.second = true;
          }
      }, response.body);

      return body;
    }
  };
}
#endif //HARBOR_CLS_HTTPRESPONSECOMPOSER_HPP
