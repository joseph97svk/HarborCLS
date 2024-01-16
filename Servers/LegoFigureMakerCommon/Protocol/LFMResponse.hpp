//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_LFMRESPONSE_HPP
#define HARBOR_CLS_LFMRESPONSE_HPP

#include <string>
#include <variant>
#include <vector>

#include "HarborCLS.hpp"

#include "LFMProtocolHeader.hpp"

struct LFMResponse {
  std::shared_ptr<HarborCLS::TcpSocket> socket;

  LegoMessageCode messageCode;

  std::variant<std::string, std::vector<char>> responseContent;

  LFMResponse(std::shared_ptr<HarborCLS::TcpSocket> socket, LegoMessageCode messageCode, std::string responseContent)
      : socket(std::move(socket))
      , messageCode(messageCode)
      , responseContent(std::move(responseContent)) {
  }
};

#endif //HARBOR_CLS_LFMRESPONSE_HPP
