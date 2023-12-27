//
// Created by josephvalverde on 12/26/23.
//

#ifndef HARBOR_CLS_CONTROLLERMANAGER_HPP
#define HARBOR_CLS_CONTROLLERMANAGER_HPP

#include "BaseController.hpp"

namespace HarborCLS {

  class ControllerManager {
    std::vector<std::shared_ptr<BaseController>> _controllers {};

  public:
    ControllerManager() = default;

    template<typename Controller>
    void addController() {
      _controllers.push_back(std::make_shared<Controller>());
    }
  };

}

#endif //HARBOR_CLS_CONTROLLERMANAGER_HPP
