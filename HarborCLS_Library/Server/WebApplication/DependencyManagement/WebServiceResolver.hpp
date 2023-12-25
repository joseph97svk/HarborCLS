//
// Created by josephvalverde on 12/24/23.
//

#ifndef HARBOR_CLS_WEBSERVICERESOLVER_HPP
#define HARBOR_CLS_WEBSERVICERESOLVER_HPP

#include <memory>
#include "IWebServiceResolver.hpp"

namespace HarborCLS {

  template <ServerProtocol Protocol>
  class WebServiceResolver : public IWebServiceResolver<Protocol> {
    using RequestType = typename Protocol::RequestType;

  public:
    std::shared_ptr<BaseWebAppService<Protocol>> resolve(
        std::shared_ptr<RequestType> request,
        std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>& services) {
      if (services.empty()) {
        return nullptr;
      }

      for (auto& service : services) {
        if (service->canHandle(request)) {
          return service;
        }
      }

      return services[0];
    }
  };
}


#endif //HARBOR_CLS_WEBSERVICERESOLVER_HPP
