//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_LFMREQUEST_HPP
#define HARBOR_CLS_LFMREQUEST_HPP

#include <string>
#include "LFMProtocolHeader.hpp"

struct LFMRequest {
  LegoMessageCode messageCode;

  std::string figure;
};

#endif //HARBOR_CLS_LFMREQUEST_HPP
