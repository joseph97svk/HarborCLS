//
// Created by josephvalverde on 12/16/23.
//

#ifndef LEGO_FIGURE_MAKER_HTTPHEADERPARSER_HPP
#define LEGO_FIGURE_MAKER_HTTPHEADERPARSER_HPP

#include <unordered_map>
#include "Http/HttpMessages/HttpHeader.hpp"
#include "Http/HttpMessages/HttpMappings.hpp"

class HttpHeaderParser {
  static std::tuple<HttpMappings::Method, std::string, std::string> parseRequestLine(std::string& requestLine);
public:
  static HttpHeader parseHeader(std::string& header);

  static bool isHeaderValid(std::string& header);
};


#endif //LEGO_FIGURE_MAKER_HTTPHEADERPARSER_HPP
