//
// Created by josephvalverde on 12/16/23.
//

#ifndef LEGO_FIGURE_MAKER_HTTPHEADER_HPP
#define LEGO_FIGURE_MAKER_HTTPHEADER_HPP

#include <string>
#include <unordered_map>

#include "HttpMappings.hpp"

struct HttpHeader {
  std::string rawContent {};

  HttpMappings::Method method {};
  std::string url {};
  std::string httpVersion {};

  std::unordered_map<HttpMappings::HeaderField, std::string> fields {};
};

#endif //LEGO_FIGURE_MAKER_HTTPHEADER_HPP
