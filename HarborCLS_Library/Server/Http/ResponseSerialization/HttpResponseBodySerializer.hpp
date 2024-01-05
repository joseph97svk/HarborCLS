//
// Created by josephvalverde on 1/4/24.
//

#ifndef HARBOR_CLS_HTTPRESPONSEBODYSERIALIZER_HPP
#define HARBOR_CLS_HTTPRESPONSEBODYSERIALIZER_HPP

#include <vector>
#include <variant>

#include "../../Protocols/IResponseBodySerializer.hpp"
#include "../HttpMessages/Reponse/HttpResponse.hpp"

namespace HarborCLS {

class HttpResponseBodySerializer : public IResponseBodySerializer<HttpResponse> {
public:
  [[nodiscard]] std::vector<char> serialize(HttpResponse& response) override {
    std::vector<char> responseVector;

    std::visit(overloaded{
        [&responseVector](std::string &str) {
          responseVector = std::vector<char>(str.begin(), str.end());
        },
        [&responseVector](std::vector<char> &vec) {
          responseVector = std::move(vec);
        }
    }, response.body);

    return responseVector;
  }
};

}

#endif //HARBOR_CLS_HTTPRESPONSEBODYSERIALIZER_HPP
