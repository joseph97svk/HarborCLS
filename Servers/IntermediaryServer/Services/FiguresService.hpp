//
// Created by josephvalverde on 1/15/24.
//

#ifndef HARBOR_CLS_FIGURESSERVICE_HPP
#define HARBOR_CLS_FIGURESSERVICE_HPP

#include <memory>
#include <vector>
#include <iostream>

#include "RoutingService.hpp"

class FiguresService {
  std::shared_ptr<RoutingService> _routingMapService;

public:
  explicit FiguresService(std::shared_ptr<RoutingService> routingMapService)
      : _routingMapService(std::move(routingMapService)) {
  }

  std::vector<std::string> getFigures() {
    return _routingMapService->getAllFigures();
  }

  void print() {
    std::cout << _routingMapService->toString() << std::endl;
  }
};

#endif //HARBOR_CLS_FIGURESSERVICE_HPP
