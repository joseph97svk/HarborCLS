//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_HTTPRESPONSEHEADERCOMPOSER_HPP
#define HARBOR_CLS_HTTPRESPONSEHEADERCOMPOSER_HPP

#include "IResponseHeaderComposer.hpp"

namespace HarborCLS {

  class HttpResponseHeaderComposer : public IResponseHeaderComposer {
  public:
    std::string composeHeader(std::shared_ptr<HttpResponse> response) override {
      std::string header;

      header += response->htmlVersion + " ";
      header += HttpResponseStatusCode::getStatusCodeString(response->statusCode) + "\r\n";

      if (contentTypeMap.contains(response->contentType)) {
        header += "Content-Type: " + contentTypeMap.at(response->contentType) + "\r\n";
      }

      header += "Content-Length: " + std::to_string(response->contentLength) + "\r\n";

      for (auto &field: response->otherHeaderFields) {
        header += field.fieldName + ": " + field.value + "\r\n";
      }

      header += HttpMappings::separator;

      return header;
    }
  };
}
#endif //HARBOR_CLS_HTTPRESPONSEHEADERCOMPOSER_HPP
