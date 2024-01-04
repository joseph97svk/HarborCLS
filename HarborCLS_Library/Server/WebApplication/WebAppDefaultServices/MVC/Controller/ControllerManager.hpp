//
// Created by josephvalverde on 12/26/23.
//

#ifndef HARBOR_CLS_CONTROLLERMANAGER_HPP
#define HARBOR_CLS_CONTROLLERMANAGER_HPP

#include "BaseController.hpp"
#include "IControllerRegistration.hpp"
#include "ControllerRegistration.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class ControllerManager {
    std::unordered_map<std::string, std::unique_ptr<IControllerRegistration<Protocol>>> _controllerRegistrations {};

    Builder<Protocol>& _dependencyResolver;

  public:
    explicit ControllerManager(Builder<Protocol>& dependencyResolver)
        : _dependencyResolver(dependencyResolver) {}

    template<typename Controller>
    requires std::is_base_of<BaseController<Protocol>, Controller>::value
    void addController(const std::string controllerName) {
      _controllerRegistrations.insert({
          controllerName
          , std::make_unique<ControllerRegistration<Protocol, Controller>>(_dependencyResolver)
      });

      _dependencyResolver.template addScoped<Controller>();
    }

    std::optional<std::shared_ptr<BaseController<Protocol>>> getController(std::string& controllerName) {
      if (_controllerRegistrations.contains(controllerName)) {
        return _controllerRegistrations[controllerName]->getController();
      }

      return std::nullopt;
    }

  };

}

#endif //HARBOR_CLS_CONTROLLERMANAGER_HPP
