//
// Created by josephvalverde on 12/16/23.
//

#include <sstream>
#include "HttpHeaderParser.hpp"

namespace HarborCLS {

  HttpHeader HttpHeaderParser::parseHeader(std::string &header) {
    HttpMappings &httpMappings = HttpMappings::getInstance();

    std::stringstream headerStream(header);

    HttpHeader httpHeader;
    httpHeader.rawContent = header;

    std::string line;
    std::getline(headerStream, line);

    std::tie(httpHeader.method, httpHeader.url, httpHeader.httpVersion) = parseRequestLine(line);

    while (headerStream.good()) {
      line.clear();
      std::getline(headerStream, line);
      std::istringstream lineStream(line);

      std::string key;
      lineStream >> key;
      key = key.substr(0, key.size() - 1);

      if (!httpMappings.headerFields.contains(key)) {
        continue;
      }

      std::string value;
      lineStream >> value;

      HttpMappings::HeaderField headerField = httpMappings.headerFields.at(key);

      httpHeader.fields[headerField] = value;
    }

    return httpHeader;
  }

  bool HttpHeaderParser::isHeaderValid(std::string &header) {
    (void) header;

    return false;
  }

  std::tuple<HttpMappings::Method, std::string, std::string>
  HttpHeaderParser::parseRequestLine(std::string &requestLine) {
    std::istringstream iss(requestLine);

    std::string methodStr;
    iss >> methodStr;
    HttpMappings::Method method = HttpMappings::getInstance().methods.at(methodStr);

    std::string url;
    iss >> url;

    std::string httpVersion;
    iss >> httpVersion;

    return {method, url, httpVersion};
  }
}