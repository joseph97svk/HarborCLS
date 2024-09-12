//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_LIMITEDSCOPEREGISTRATION_HPP
#define HARBOR_CLS_LIMITEDSCOPEREGISTRATION_HPP

#include "Hypodermic/Hypodermic/ContainerBuilder.h"

#include "LimitedScopeInstanceTracing.hpp"

template<typename ScopedType>
class LimitedScopeRegistration {
  using Type = Hypodermic::RegistrationDescriptorBuilder::ForTypeConstruction<ScopedType>::Type;

  std::shared_ptr<LimitedScopeInstanceTracing<ScopedType>> _instanceTracing;

public:
  explicit LimitedScopeRegistration(Type& ref)
      : _instanceTracing(std::make_shared<LimitedScopeInstanceTracing<ScopedType>>(ref)) {
  }

  template<typename interface>
  void as() {
    _instanceTracing->template as<interface>();
  }
};

#endif //HARBOR_CLS_LIMITEDSCOPEREGISTRATION_HPP
