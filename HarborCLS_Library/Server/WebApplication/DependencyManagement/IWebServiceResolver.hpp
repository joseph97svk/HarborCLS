//
// Created by josephvalverde on 12/24/23.
//

#ifndef HARBOR_CLS_IWEBSERVICERESOLVER_HPP
#define HARBOR_CLS_IWEBSERVICERESOLVER_HPP

#include "../../Protocols/ProtocolConcept.hpp"
#include "BaseWebAppService.hpp"

namespace HarborCLS {

  template <ServerProtocol Protocol>
  class IWebServiceResolver {
    using RequestType = typename Protocol::RequestType;
  public:
    virtual ~IWebServiceResolver() = default;

    virtual std::shared_ptr<BaseWebAppService<Protocol>> resolve(
        std::shared_ptr<RequestType> request,
        std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>& services) = 0;
  };
}

#endif //HARBOR_CLS_IWEBSERVICERESOLVER_HPP
