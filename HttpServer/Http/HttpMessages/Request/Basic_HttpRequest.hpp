//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_BASIC_HTTPREQUEST_HPP
#define LEGO_FIGURE_MAKER_BASIC_HTTPREQUEST_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "Http/HttpMessages/HttpHeader.hpp"

template<typename SocketType>
struct Basic_HttpRequest {
  std::shared_ptr<SocketType> socket {};

  HttpHeader header {};

  std::shared_ptr<std::variant<std::string, std::vector<char>>> body {};

  Basic_HttpRequest() = default;

  Basic_HttpRequest(std::shared_ptr<SocketType> socket,
                    HttpHeader header,
                    std::shared_ptr<std::variant<std::string, std::vector<char>>> body)
    : socket(std::move(socket))
    , header(std::move(header))
    , body(std::move(body)) {}
};


#endif //LEGO_FIGURE_MAKER_BASIC_HTTPREQUEST_HPP
