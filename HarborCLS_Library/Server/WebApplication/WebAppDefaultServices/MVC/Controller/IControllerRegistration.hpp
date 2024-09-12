//
// Created by josephvalverde on 1/3/24.
//

#ifndef HARBOR_CLS_ICONTROLLERREGISTRATION_HPP
#define HARBOR_CLS_ICONTROLLERREGISTRATION_HPP

#include <memory>

#include "BaseController.hpp"
#include "../../../../Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class IControllerRegistration {
  public:
    virtual ~IControllerRegistration() = default;

    [[nodiscard]] virtual std::shared_ptr<BaseController<Protocol>> getController() = 0;
  };

}

#endif //HARBOR_CLS_ICONTROLLERREGISTRATION_HPP
