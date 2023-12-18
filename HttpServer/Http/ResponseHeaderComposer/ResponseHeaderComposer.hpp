//
// Created by josephvalverde on 12/18/23.
//

#ifndef LEGO_FIGURE_MAKER_RESPONSEHEADERCOMPOSER_HPP
#define LEGO_FIGURE_MAKER_RESPONSEHEADERCOMPOSER_HPP

#include "IResponseHeaderComposer.hpp"

class ResponseHeaderComposer : public IResponseHeaderComposer {
public:
    std::string composeHeader(std::shared_ptr<HttpResponse> response) override {
      std::string header;

      header += response->htmlVersion + " ";
      header += HttpResponseStatusCode::getStatusCodeString(response->statusCode) + "\r\n";

      if (contentTypeMap.contains(response->contentType)) {
        header += "Content-Type: " + contentTypeMap.at(response->contentType) + "\r\n";
      }

      header += "Content-Length: " + std::to_string(response->contentLength) + "\r\n";

      for (auto& field : response->otherHeaderFields) {
        header += field.fieldName + ": " + field.value + "\r\n";
      }

      return header;
    }
};

#endif //LEGO_FIGURE_MAKER_RESPONSEHEADERCOMPOSER_HPP
