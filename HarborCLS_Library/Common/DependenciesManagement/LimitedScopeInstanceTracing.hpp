//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_LIMITEDSCOPEINSTANCETRACING_HPP
#define HARBOR_CLS_LIMITEDSCOPEINSTANCETRACING_HPP

#include <Hypodermic/RegistrationDescriptorBuilder.h>
#include "Hypodermic/Hypodermic/ContainerBuilder.h"

template<typename ScopedType>
struct LimitedScopeInstanceTracing {
  using Type = Hypodermic::RegistrationDescriptorBuilder::ForTypeConstruction<ScopedType>::Type;

  Type& ref;

  bool _alreadyInstancedAsSingleton = false;

  explicit LimitedScopeInstanceTracing(Type& ref)
      : ref(ref) {
  }

  template <typename interface>
  void as() {
    ref.template as<interface>().singleInstance();
  }

  ~LimitedScopeInstanceTracing() {
    if (_alreadyInstancedAsSingleton) {
      return;
    }

    ref.singleInstance();
  }
};

#endif //HARBOR_CLS_LIMITEDSCOPEINSTANCETRACING_HPP
