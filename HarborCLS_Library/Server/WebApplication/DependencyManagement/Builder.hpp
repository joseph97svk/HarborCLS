//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_BUILDER_HPP
#define HARBOR_CLS_BUILDER_HPP

#include <type_traits>

#include "ApplicationStartupTaskSet.hpp"
#include "../../../Common/DependenciesManagement/DependencyManager.hpp"
#include "BaseWebAppService.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class Builder : public DependencyManager {
  protected:
    std::shared_ptr<ApplicationStartupTaskSet <Hypodermic::Container, Protocol>> _applicationStartupTaskSet;

  public:
    Builder()
        : DependencyManager()
        , _applicationStartupTaskSet(std::make_shared<ApplicationStartupTaskSet <Hypodermic::Container, Protocol>>()) {
    }

    template<typename T>
    Type<T>& addOnStart(auto entryFunction) {
      std::unique_ptr<Task < T, decltype(entryFunction), Hypodermic::Container>> task =
          std::make_unique<Task < T, decltype(entryFunction), Hypodermic::Container>>(entryFunction);

      _applicationStartupTaskSet->addInitTask(std::move(task));

      return _taskBuilder.template registerType<T>();
    }

    template<typename T>
    Type<T>& addLivingTask() {
      _applicationStartupTaskSet->addLivingTask(
          std::make_unique<LivingTask<Protocol, Hypodermic::Container, T>>());

      return _taskBuilder.template registerType<T>();
    }

    std::shared_ptr<ApplicationStartupTaskSet <Hypodermic::Container, Protocol>> getApplicationStartupTaskSet() {
      std::shared_ptr<Hypodermic::Container> globalContainer =
          _globalBuilder.build();
      std::shared_ptr<Hypodermic::Container> nestedContainer =
          _taskBuilder.buildNestedContainerFrom(*globalContainer);
      std::shared_ptr<Hypodermic::Container> scopedContainer =
          _scopedBuilder.buildNestedContainerFrom(*nestedContainer);

      _applicationStartupTaskSet->addContainer(scopedContainer);

      return _applicationStartupTaskSet;
    }
  };
}

#endif //HARBOR_CLS_BUILDER_HPP
