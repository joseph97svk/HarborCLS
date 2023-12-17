//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_IHTTPREQUESTPARSER_HPP
#define LEGO_FIGURE_MAKER_IHTTPREQUESTPARSER_HPP

#include <memory>

#include "HttpMessages/HttpRequest.hpp"

template <typename SocketType>
class IHttpRequestParser {
public:
    virtual ~IHttpRequestParser() = default;

  [[nodiscard]] virtual std::shared_ptr<HttpRequest> createHttpRequest(
          std::vector<char>& request,
          std::shared_ptr<SocketType>) = 0;
};

#endif //LEGO_FIGURE_MAKER_IHTTPREQUESTPARSER_HPP
