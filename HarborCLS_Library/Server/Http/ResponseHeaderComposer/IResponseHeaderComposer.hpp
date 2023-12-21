//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_IRESPONSEHEADERCOMPOSER_HPP
#define HARBOR_CLS_IRESPONSEHEADERCOMPOSER_HPP

#include <string>

#include "../HttpMessages/Reponse/HttpResponse.hpp"

namespace HarborCLS {

  class IResponseHeaderComposer {
  public:
    virtual ~IResponseHeaderComposer() = default;

    virtual std::string composeHeader(std::shared_ptr<HttpResponse>) = 0;
  };
}

#endif //HARBOR_CLS_IRESPONSEHEADERCOMPOSER_HPP
