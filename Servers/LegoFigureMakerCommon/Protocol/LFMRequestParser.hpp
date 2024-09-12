//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_LFMREQUESTPARSER_HPP
#define HARBOR_CLS_LFMREQUESTPARSER_HPP

#include "HarborCLS.hpp"

#include "LFMRequest.hpp"

class LFMRequestParser : public HarborCLS::IRequestParser<HarborCLS::TcpSocket, LFMRequest> {
public:
  [[nodiscard]] std::shared_ptr<LFMRequest> createRequest(
      std::vector<char> &request,
      std::shared_ptr<HarborCLS::TcpSocket> socket) override {

    std::string rawCode { request[0] };
    LegoMessageCode messageCode = static_cast<LegoMessageCode>(std::stoi(rawCode));

    std::string figure = std::string(request.begin() + 2, request.end());

    return std::make_shared<LFMRequest>(
        LFMRequest {
            socket,
            messageCode,
            figure
        });
  }
};

#endif //HARBOR_CLS_LFMREQUESTPARSER_HPP
