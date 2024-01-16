//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_ROUTINGMAPSERVICE_HPP
#define HARBOR_CLS_ROUTINGMAPSERVICE_HPP

#include <mutex>
#include <unordered_map>

#include "../../LegoFigureMakerCommon/ServerRoutingInfo.hpp"

class RoutingMapService {
  using RoutingMapIterator = std::unordered_map<std::string, ServerRoutingInfo>::iterator;

  std::unordered_map<std::string, ServerRoutingInfo> _routingMap;

  std::unordered_map<ServerRoutingInfo, RoutingMapIterator> _reverseRoutingMap;

  std::mutex _canAccessRoutingMap;

public:
  ServerRoutingInfo& operator[](std::string& figure);

  size_t count(std::string& figure);

  void insert(std::pair<std::string, ServerRoutingInfo> figureStructure);

  void erase(std::string& figure);

  std::string toString();

  void erase(std::pair<std::string, int>& pairToDelete);

  void erase(const ServerRoutingInfo& routingInfo);

  RoutingMapIterator begin();

  RoutingMapIterator end() ;
};

#endif //HARBOR_CLS_ROUTINGMAPSERVICE_HPP
