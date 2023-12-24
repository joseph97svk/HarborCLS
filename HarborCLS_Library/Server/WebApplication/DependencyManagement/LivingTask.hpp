//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_LIVINGTASK_HPP
#define HARBOR_CLS_LIVINGTASK_HPP

#include "ILivingTask.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol, typename DependencyProviderContainer, typename ClassType>
  class LivingTask : public ILivingTask<Protocol, DependencyProviderContainer> {
    std::shared_ptr<BaseWebAppService<Protocol>> _taskClass;

  public:
    void set(std::shared_ptr<DependencyProviderContainer> container) override {
      _taskClass = container->template resolve<ClassType>();
    }

    std::shared_ptr<BaseWebAppService<Protocol>> get() override {
      return _taskClass;
    }
  };
}
#endif //HARBOR_CLS_LIVINGTASK_HPP
