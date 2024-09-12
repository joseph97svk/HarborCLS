//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_TASK_HPP
#define HARBOR_CLS_TASK_HPP

#include "ITask.hpp"

namespace HarborCLS {

  template<typename Class, typename EntryFunction, typename DependencyProviderContainer>
  class Task : public ITask<DependencyProviderContainer> {
    EntryFunction _entryFunction;

    std::shared_ptr<Class> _taskClass;

    std::function<std::shared_ptr<Class>(DependencyProviderContainer&)> _create;

  public:
    explicit Task(EntryFunction entryFunction
                  , std::function<std::shared_ptr<Class>(DependencyProviderContainer&)> create)
        : _entryFunction(entryFunction)
        , _create(create) {
    }

    void run(DependencyProviderContainer& container) override {
      _taskClass = _create(container);

      if (_taskClass) {
        ((*_taskClass).*_entryFunction)();
      }
    }
  };

}
#endif //HARBOR_CLS_TASK_HPP
