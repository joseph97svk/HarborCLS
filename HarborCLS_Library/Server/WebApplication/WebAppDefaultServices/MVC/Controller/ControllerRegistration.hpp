//
// Created by josephvalverde on 1/3/24.
//

#ifndef HARBOR_CLS_CONTROLLERREGISTRATION_HPP
#define HARBOR_CLS_CONTROLLERREGISTRATION_HPP

#include "IControllerRegistration.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol, typename Controller>
  class ControllerRegistration : public IControllerRegistration<Protocol> {
    Builder<Protocol>& _dependencyResolver;
  public:
    explicit ControllerRegistration(Builder<Protocol>& dependencyResolver)
        :_dependencyResolver(dependencyResolver) {
    }

    [[nodiscard]] std::shared_ptr<BaseController<Protocol>> getController() override {
      auto scopedContainer = _dependencyResolver.createScopedContainer();

      return scopedContainer->template resolve<Controller>();
    }
  };

}

#endif //HARBOR_CLS_CONTROLLERREGISTRATION_HPP
