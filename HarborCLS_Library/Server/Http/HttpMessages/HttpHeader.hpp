//
// Created by josephvalverde on 12/16/23.
//

#ifndef HARBOR_CLS_HTTPHEADER_HPP
#define HARBOR_CLS_HTTPHEADER_HPP

#include <string>
#include <unordered_map>

#include "HttpMappings.hpp"

namespace HarborCLS {

  struct HttpHeader {
    std::string rawContent{};

    HttpMappings::Method method{};
    std::string url{};
    std::string httpVersion{};

    std::unordered_map<HttpMappings::HeaderField, std::string> fields{};
  };
}

#endif //HARBOR_CLS_HTTPHEADER_HPP
