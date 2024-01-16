//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_ROUTINGSERVICE_HPP
#define HARBOR_CLS_ROUTINGSERVICE_HPP

#include <vector>
#include <iostream>

#include "Utility/RoutingMap.hpp"

class RoutingService {
  RoutingMap _routingMap;

public:
  size_t getCount(std::string& figure);

  void addRoute(std::pair<std::string, ServerRoutingInfo> figureStructure);

  void erase(std::string& figure);

  void erase(std::pair<std::string, int>& pairToDelete);

  void erase(const ServerRoutingInfo& routingInfo);

  std::string toString();

  std::vector<std::string> getAllFigures();
};

#endif //HARBOR_CLS_ROUTINGSERVICE_HPP
