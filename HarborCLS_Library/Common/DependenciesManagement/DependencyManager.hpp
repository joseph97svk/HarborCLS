//
// Created by josephvalverde on 12/20/23.
//

#ifndef HARBOR_CLS_DEPENDENCYMANAGER_HPP
#define HARBOR_CLS_DEPENDENCYMANAGER_HPP

#include "Hypodermic/Hypodermic/ContainerBuilder.h"
#include "ApplicationStartupTaskSet.hpp"

class DependencyManager {
protected:
  template<typename T>
  using Type = Hypodermic::RegistrationDescriptorBuilder::ForTypeConstruction< T >::Type;

  Hypodermic::ContainerBuilder _globalBuilder;
  Hypodermic::ContainerBuilder _scopedBuilder;
  Hypodermic::ContainerBuilder _taskBuilder;

  std::shared_ptr<ApplicationStartupTaskSet<Hypodermic::Container>> _applicationStartupTaskSet;

public:
  DependencyManager()
      : _globalBuilder()
      , _scopedBuilder()
      , _taskBuilder()
      , _applicationStartupTaskSet(std::make_shared<ApplicationStartupTaskSet<Hypodermic::Container>>()) {
  }

  template<typename T>
  Type<T>& addScoped() {
    return _scopedBuilder.template registerType<T>();
  }

  template<typename T>
  Type<T>& addSingleton() {
    return _globalBuilder.template registerType<T>().singleInstance();
  }

  template<typename T>
  Type<T>& addTransient() {
    return _globalBuilder.template registerType<T>();
  }

  template<typename T>
  Type<T>& addOnStart(auto entryFunction) {
    _applicationStartupTaskSet->addInitTask(
        std::make_unique<Task<T, decltype(entryFunction)
        , Hypodermic::Container>>(entryFunction));

    return _taskBuilder.template registerType<T>();
  }

  template<typename T>
  Type<T>& livingTask(auto entryFunction) {
    _applicationStartupTaskSet->addLivingTask(
        std::make_unique<Task<T, decltype(entryFunction)
        , Hypodermic::Container>>(entryFunction));

    return _taskBuilder.template registerType<T>();
  }

  std::shared_ptr<Hypodermic::Container> createScopedContainer() {
    std::shared_ptr<Hypodermic::Container> globalContainer =
        _globalBuilder.build();
    std::shared_ptr<Hypodermic::Container> nestedContainer =
        _scopedBuilder.buildNestedContainerFrom(*globalContainer);

    return nestedContainer;
  }

  std::shared_ptr<ApplicationStartupTaskSet<Hypodermic::Container>> getApplicationStartupTaskSet() {
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

#endif //HARBOR_CLS_DEPENDENCYMANAGER_HPP
