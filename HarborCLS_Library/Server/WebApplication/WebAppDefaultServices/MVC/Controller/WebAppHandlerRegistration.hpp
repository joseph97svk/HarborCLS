//
// Created by josephvalverde on 12/26/23.
//

#ifndef HARBOR_CLS_WEBAPPHANDLERREGISTRATION_HPP
#define HARBOR_CLS_WEBAPPHANDLERREGISTRATION_HPP

#include <functional>
#include <optional>
#include <memory>
#include "IWebAppHandlerRegistration.hpp"

namespace HarborCLS {

  template<typename RequestType, typename ResponseType, typename ControllerImplementation>
  class WebAppHandlerRegistration : public IWebAppHandlerRegistration<RequestType, ResponseType> {
    using HandlerFunction = std::optional<std::shared_ptr<ResponseType>> (ControllerImplementation::*)(std::shared_ptr<RequestType>);
    HandlerFunction _handlerFunction;

    ControllerImplementation& _controllerImplementation;

  public:
    explicit WebAppHandlerRegistration(
        HandlerFunction handlerFunction
        , ControllerImplementation& controllerImplementation)
            : _handlerFunction(handlerFunction)
            , _controllerImplementation(controllerImplementation) {
    }

    std::optional<std::shared_ptr<ResponseType>> executeHandler(std::shared_ptr<RequestType> request) override {
      return std::invoke(_handlerFunction, &_controllerImplementation, request);
    }
  };

}

#endif //HARBOR_CLS_WEBAPPHANDLERREGISTRATION_HPP
