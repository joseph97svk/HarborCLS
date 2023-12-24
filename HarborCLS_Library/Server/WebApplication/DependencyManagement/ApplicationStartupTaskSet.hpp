//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_APPLICATIONSTARTUPTASKSET_HPP
#define HARBOR_CLS_APPLICATIONSTARTUPTASKSET_HPP

#include <vector>

#include "Task.hpp"
#include "LivingTask.hpp"

namespace HarborCLS {

  template<typename DependencyProviderContainer, ServerProtocol Protocol>
  class ApplicationStartupTaskSet {
    std::shared_ptr<DependencyProviderContainer> _container;

    std::vector<std::unique_ptr<ITask<DependencyProviderContainer>>> _initTasks;
    std::vector<std::unique_ptr<ILivingTask<Protocol, DependencyProviderContainer>>> _livingTasks;

    std::shared_ptr<std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>> _services;

  public:
    void addContainer(std::shared_ptr<DependencyProviderContainer> container) {
      this->_container = container;
    }

    void addInitTask(std::unique_ptr<ITask<DependencyProviderContainer>> task) {
      _initTasks.push_back(std::move(task));
    }

    void addLivingTask(std::unique_ptr<ILivingTask<Protocol, DependencyProviderContainer>> livingTask) {
      _livingTasks.push_back(std::move(livingTask));
    }

    void runInitTasks() {
      if (!_container) {
        throw std::runtime_error("Container is not set");
      }

      for (auto &task: _initTasks) {
        task->run(_container);
      }
    }

    void initLivingTasks() {
      if (!_container) {
        throw std::runtime_error("Container is not set");
      }

      for (auto &task: _livingTasks) {
        task->set(_container);
      }
    }

    std::shared_ptr<std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>> getLivingTasks() {
      if (_services != nullptr) {
        return _services;
      }

      _services = std::make_shared<std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>>();

      for (auto &task: _livingTasks) {
        _services->push_back(task->get());
      }
      return _services;
    }
  };

}
#endif //HARBOR_CLS_APPLICATIONSTARTUPTASKSET_HPP
