//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_LFMRESPONSE_HPP
#define HARBOR_CLS_LFMRESPONSE_HPP

#include <string>
#include <variant>
#include <vector>
#include "LFMProtocolHeader.hpp"

struct LFMResponse {
  LegoMessageCode messageCode;

  std::variant<std::string, std::vector<char>> responseContent;
};

#endif //HARBOR_CLS_LFMRESPONSE_HPP
