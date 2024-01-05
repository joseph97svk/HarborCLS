//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_IRESPONSECOMPOSER_HPP
#define HARBOR_CLS_IRESPONSECOMPOSER_HPP

#include "../Http/HttpMessages/Reponse/HttpResponse.hpp"

namespace HarborCLS {

  template<typename ResponseType>
  class IResponseComposer {
  public:
    virtual ~IResponseComposer() = default;

    [[nodiscard]] virtual std::vector<char> compose(ResponseType& response) = 0;
  };
}

#endif //HARBOR_CLS_IRESPONSECOMPOSER_HPP
