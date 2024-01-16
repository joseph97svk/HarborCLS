//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_BUILDERREFERENCEWRAPPER_HPP
#define HARBOR_CLS_BUILDERREFERENCEWRAPPER_HPP

#include "Builder.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  struct BuilderReferenceWrapper {
    HarborCLS::Builder<Protocol> &builder;

    explicit BuilderReferenceWrapper(HarborCLS::Builder<Protocol> &builder) : builder(builder) {}
  };
}


#endif //HARBOR_CLS_BUILDERREFERENCEWRAPPER_HPP
