//
// Created by josephvalverde on 12/16/23.
//

#ifndef HARBOR_CLS_HTTPHEADERPARSER_HPP
#define HARBOR_CLS_HTTPHEADERPARSER_HPP

#include <unordered_map>

#include "../HttpMessages/HttpMappings.hpp"
#include "../HttpMessages/HttpHeader.hpp"

namespace HarborCLS {

  class HttpHeaderParser {
    static std::tuple<HttpMappings::Method, std::string, std::string> parseRequestLine(std::string &requestLine);

  public:
    static HttpHeader parseHeader(std::string &header);

    static bool isHeaderValid(std::string &header);
  };
}

#endif //HARBOR_CLS_HTTPHEADERPARSER_HPP
