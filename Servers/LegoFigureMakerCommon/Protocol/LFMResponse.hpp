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

  LFMResponse(std::shared_ptr<HarborCLS::TcpSocket> connectionSocket
              , LegoMessageCode responseMessageCode
              , std::string responseMessageContent)
      : socket(std::move(connectionSocket))
      , messageCode(responseMessageCode)
      , responseContent(std::move(responseMessageContent)) {
  }

  LFMResponse(std::shared_ptr<HarborCLS::TcpSocket> connectionSocket
              , LegoMessageCode responseMessageCode
              , std::vector<char> responseMessageContent)
      : socket(std::move(connectionSocket))
      , messageCode(responseMessageCode)
      , responseContent(std::move(responseMessageContent)) {
  }
};

#endif //HARBOR_CLS_LFMRESPONSE_HPP
