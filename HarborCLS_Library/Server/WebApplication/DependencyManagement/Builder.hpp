//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_BUILDER_HPP
#define HARBOR_CLS_BUILDER_HPP

#include <type_traits>

#include "ApplicationStartupTaskSet.hpp"
#include "../../../Common/DependenciesManagement/DependencyManager.hpp"
#include "BaseWebAppService.hpp"
#include "../../../../Servers/PiecesServer/Services/LegoPresentService.hpp"

#include "ContainerBundle.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class Builder : public DependencyManager {
  protected:
    ApplicationStartupTaskSet <Hypodermic::Container, Protocol> _applicationStartupTaskSet {};

    bool _applicationStartupTaskSetIsSet { false };

  public:
    Builder()
        : DependencyManager()
        {
    }

    template<typename T>
    Type<T>& addOnStart(auto entryFunction) {
      std::unique_ptr<Task < T, decltype(entryFunction), Hypodermic::Container>> task =
          std::make_unique<Task < T, decltype(entryFunction), Hypodermic::Container>>(
              entryFunction
              , [](Hypodermic::Container& container) {
                return container.resolve<T>();
              });

      _applicationStartupTaskSet.addInitTask(std::move(task));

      return _taskBuilder.template registerType<T>();
    }

    template<typename T>
    Type<T>& addLivingTask() {
      _applicationStartupTaskSet.addLivingTask(
          std::make_unique<LivingTask<Protocol, Hypodermic::Container, T>>());

      return _taskBuilder.template registerType<T>();
    }

    ApplicationStartupTaskSet <Hypodermic::Container, Protocol>& getApplicationStartupTaskSet() {
      if (!_applicationStartupTaskSetIsSet) {
        this->setApplicationDataSetContainer();
      }

      return _applicationStartupTaskSet;
    }

  protected:
    void setApplicationDataSetContainer() {
      std::shared_ptr<Hypodermic::Container> globalContainer = _globalBuilder.build();
      std::shared_ptr<Hypodermic::Container> scopedContainer =
          _scopedBuilder.buildNestedContainerFrom(*globalContainer);
      std::shared_ptr<Hypodermic::Container> tasksContainer =
          _taskBuilder.buildNestedContainerFrom(*scopedContainer);

      ContainerBundle<Hypodermic::Container> tasksContainerBundle {
          ._globalContainer = globalContainer
          , ._scopedContainer = scopedContainer
          , ._tasksContainer = tasksContainer
      };

      _applicationStartupTaskSet.addContainer(tasksContainerBundle);
    }
  };
}

#endif //HARBOR_CLS_BUILDER_HPP
