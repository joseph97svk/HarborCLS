//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_LFMRESPONSECOMPOSER_HPP
#define HARBOR_CLS_LFMRESPONSECOMPOSER_HPP

#include "HarborCLS.hpp"

#include "LFMResponse.hpp"

class LFMResponseComposer : public HarborCLS::IResponseComposer<LFMResponse> {
public:
  [[nodiscard]] std::vector<char> compose(LFMResponse& response) override {
    char code =  std::to_string(response.messageCode)[0];
    char separator = std::to_string(SEPARATOR)[0];

    std::vector<char> composedResponse {code, separator};

    std::visit(overloaded {
      [&composedResponse](std::string& str) {
        composedResponse.insert(composedResponse.end(), str.begin(), str.end());
      },
      [&composedResponse](std::vector<char>& vec) {
        composedResponse.insert(composedResponse.end(), vec.begin(), vec.end());
      }
    }, response.responseContent);

    return composedResponse;
  }
};

#endif //HARBOR_CLS_LFMRESPONSECOMPOSER_HPP
