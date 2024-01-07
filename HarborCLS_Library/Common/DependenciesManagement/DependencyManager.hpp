//
// Created by josephvalverde on 12/20/23.
//

#ifndef HARBOR_CLS_DEPENDENCYMANAGER_HPP
#define HARBOR_CLS_DEPENDENCYMANAGER_HPP

#include "Hypodermic/Hypodermic/ContainerBuilder.h"
#include "Server/WebApplication/DependencyManagement/ApplicationStartupTaskSet.hpp"

namespace HarborCLS {

  class DependencyManager {
  protected:
    template<typename T>
    using Type = Hypodermic::RegistrationDescriptorBuilder::ForTypeConstruction<T>::Type;

    template<typename T>
    using InstanceType = Hypodermic::RegistrationDescriptorBuilder::ForProvidedInstance<T>::Type;

    Hypodermic::ContainerBuilder _globalBuilder;
    Hypodermic::ContainerBuilder _scopedBuilder;
    Hypodermic::ContainerBuilder _taskBuilder;

  public:
    DependencyManager()
        : _globalBuilder(), _scopedBuilder(), _taskBuilder() {}

    template<typename T>
    Type<T>& addScoped() {
      Type<T>& ref = _scopedBuilder.template registerType<T>();
      ref.singleInstance();

      return ref;
    }

    template<typename T>
    Type<T>& addSingleton() {
      Type<T>& ref = _globalBuilder.template registerType<T>().singleInstance();
      ref.singleInstance();

      return ref;
    }

    template<typename T>
    Type<T>& addTransient() {
      return _globalBuilder.template registerType<T>();
    }

    template<typename T>
    InstanceType<T>& addInstance(std::shared_ptr<T>& instance) {
      return _globalBuilder.registerInstance<T>(instance);
    }

    std::shared_ptr<Hypodermic::Container> createScopedContainer() {
      std::shared_ptr<Hypodermic::Container> globalContainer =
          _globalBuilder.build();
      std::shared_ptr<Hypodermic::Container> nestedContainer =
          _scopedBuilder.buildNestedContainerFrom(*globalContainer);

      return nestedContainer;
    }
  };
}
#endif //HARBOR_CLS_DEPENDENCYMANAGER_HPP
