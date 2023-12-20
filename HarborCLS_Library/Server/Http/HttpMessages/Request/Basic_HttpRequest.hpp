//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_BASIC_HTTPREQUEST_HPP
#define HARBOR_CLS_BASIC_HTTPREQUEST_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "../HttpHeader.hpp"

namespace HarborCLS {

  template<typename SocketType>
  struct Basic_HttpRequest {
    std::shared_ptr<SocketType> socket{};

    HttpHeader header{};

    std::shared_ptr<std::variant<std::string, std::vector<char>>> body{};

    Basic_HttpRequest() = default;

    Basic_HttpRequest(std::shared_ptr<SocketType> socket,
                      HttpHeader header,
                      std::shared_ptr<std::variant<std::string, std::vector<char>>> body)
        : socket(std::move(socket)), header(std::move(header)), body(std::move(body)) {}
  };
}

#endif //HARBOR_CLS_BASIC_HTTPREQUEST_HPP
