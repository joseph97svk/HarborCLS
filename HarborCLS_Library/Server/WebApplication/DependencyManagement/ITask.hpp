//
// Created by josephvalverde on 12/24/23.
//

#ifndef HARBOR_CLS_ITASK_HPP
#define HARBOR_CLS_ITASK_HPP

#include <memory>

namespace HarborCLS {

  template<typename DependencyProviderContainer>
  class ITask {
  public:
    virtual void run(std::shared_ptr<DependencyProviderContainer> container) = 0;

    virtual ~ITask() = default;
  };

}

#endif //HARBOR_CLS_ITASK_HPP
