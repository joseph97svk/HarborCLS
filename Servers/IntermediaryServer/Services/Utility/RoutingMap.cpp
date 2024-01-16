//
// Created by josephvalverde on 1/15/24.
//

#include "RoutingMap.hpp"

using RoutingMapIterator = std::unordered_map<std::string, ServerRoutingInfo>::iterator;

ServerRoutingInfo& RoutingMap::operator[](std::string& figure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap[figure];
}

size_t RoutingMap::count(std::string& figure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap.count(figure);
}

void RoutingMap::insert(std::pair<std::string, ServerRoutingInfo> figureStructure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);
  _routingMap.insert(figureStructure);
}

void RoutingMap::erase(std::string& figure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);
  _routingMap.erase(figure);
}

void RoutingMap::erase(std::pair<std::string, int>& pairToDelete) {
  this->erase({
                  .ip = pairToDelete.first
                  , .port = pairToDelete.second});
}

void RoutingMap::erase(const ServerRoutingInfo& routingInfo) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  auto it = _reverseRoutingMap[routingInfo];

  _routingMap.erase(it);
}

RoutingMapIterator RoutingMap::begin() {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap.begin();
}

RoutingMapIterator RoutingMap::end() {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap.end();
}