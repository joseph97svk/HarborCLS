//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_TASK_HPP
#define HARBOR_CLS_TASK_HPP

#include <memory>

template<typename DependencyProviderContainer>
class ITask {
public:
  virtual void run(std::shared_ptr<DependencyProviderContainer> container) = 0;
  virtual ~ITask() = default;
};

template<typename Class, typename EntryFunction, typename DependencyProviderContainer>
class Task : public ITask<DependencyProviderContainer> {
  EntryFunction _entryFunction;

  std::shared_ptr<Class> _taskClass;

public:
  explicit Task(EntryFunction entryFunction)
      : _entryFunction(entryFunction) {
  }

  void run(std::shared_ptr<DependencyProviderContainer> container) override{
    _taskClass = container->template resolve<Class>();

    if (_taskClass) {
      ((*_taskClass).*_entryFunction)();
    }
  }
};

#endif //HARBOR_CLS_TASK_HPP
