//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_APPLICATIONSTARTUPTASKSET_HPP
#define HARBOR_CLS_APPLICATIONSTARTUPTASKSET_HPP

#include <vector>

#include "Task.hpp"

template<typename DependencyProviderContainer>
class ApplicationStartupTaskSet {
  std::shared_ptr<DependencyProviderContainer> _container;

  std::vector<std::unique_ptr<ITask<DependencyProviderContainer>>> _initTasks;
  std::vector<std::unique_ptr<ITask<DependencyProviderContainer>>> _livingTasks;

public:
  void addContainer(std::shared_ptr<DependencyProviderContainer> container) {
    this->_container = container;
  }

  void addInitTask(std::unique_ptr<ITask<DependencyProviderContainer>> task) {
    _initTasks.push_back(std::move(task));
  }

  void addLivingTask(std::unique_ptr<ITask<DependencyProviderContainer>> task) {
    _livingTasks.push_back(std::move(task));
  }

  void runInitTasks() {
    for (auto& task : _initTasks) {
      task->run(_container);
    }
  }

  void runLivingTasks() {
    for (auto& task : _livingTasks) {
      task->run(_container);
    }
  }
};

#endif //HARBOR_CLS_APPLICATIONSTARTUPTASKSET_HPP
