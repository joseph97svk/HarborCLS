//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_IRESPONSEHEADERCOMPOSER_HPP
#define HARBOR_CLS_IRESPONSEHEADERCOMPOSER_HPP

#include "../../../../../../../usr/include/c++/13/string"

#include "../Http/HttpMessages/Reponse/HttpResponse.hpp"

namespace HarborCLS {

  template<typename ResponseType>
  class IResponseHeaderComposer {
  public:
    virtual ~IResponseHeaderComposer() = default;

    virtual std::string composeHeader(ResponseType& response) = 0;
  };
}

#endif //HARBOR_CLS_IRESPONSEHEADERCOMPOSER_HPP
