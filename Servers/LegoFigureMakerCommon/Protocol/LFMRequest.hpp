//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_LFMREQUEST_HPP
#define HARBOR_CLS_LFMREQUEST_HPP

#include <string>
#include <memory>

#include "HarborCLS.hpp"

#include "LFMProtocolHeader.hpp"

struct LFMRequest {
  std::shared_ptr<HarborCLS::TcpSocket> socket;

  LegoMessageCode messageCode;

  std::string figure;
};

#endif //HARBOR_CLS_LFMREQUEST_HPP
