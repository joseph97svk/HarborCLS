//
// Created by josephvalverde on 1/15/24.
//

#ifndef HARBOR_CLS_CONTAINERBUNDLE_HPP
#define HARBOR_CLS_CONTAINERBUNDLE_HPP

#include <memory>

namespace HarborCLS {
  template<typename ContainerType>
  struct ContainerBundle {
    std::shared_ptr<ContainerType> _globalContainer;
    std::shared_ptr<ContainerType> _scopedContainer;
    std::shared_ptr<ContainerType> _tasksContainer;
  };

}

#endif //HARBOR_CLS_CONTAINERBUNDLE_HPP
