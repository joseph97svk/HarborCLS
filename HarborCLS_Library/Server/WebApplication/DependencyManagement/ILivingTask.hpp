//
// Created by josephvalverde on 12/24/23.
//

#ifndef HARBOR_CLS_ILIVINGTASK_HPP
#define HARBOR_CLS_ILIVINGTASK_HPP

#include <memory>

#include "../../Protocols/ProtocolConcept.hpp"
#include "BaseWebAppService.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol, typename DependencyProviderContainer>
  class ILivingTask {
  public:
    virtual ~ILivingTask() = default;

    virtual void set(std::shared_ptr<DependencyProviderContainer> container) = 0;

    virtual std::shared_ptr<BaseWebAppService<Protocol>> get() = 0;
  };

}
#endif //HARBOR_CLS_ILIVINGTASK_HPP
