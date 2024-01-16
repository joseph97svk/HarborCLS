//
// Created by josephvalverde on 1/12/24.
//

#include "RoutingMapService.hpp"

using RoutingMapIterator = std::unordered_map<std::string, ServerRoutingInfo>::iterator;

ServerRoutingInfo& RoutingMapService::operator[](std::string& figure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap[figure];
}

size_t RoutingMapService::count(std::string& figure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap.count(figure);
}

void RoutingMapService::insert(std::pair<std::string, ServerRoutingInfo> figureStructure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);
  _routingMap.insert(figureStructure);
}

void RoutingMapService::erase(std::string& figure) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);
  _routingMap.erase(figure);
}

std::string RoutingMapService::toString() {
  std::string mapString;

  for (auto& pair : _routingMap) {
    mapString += "Figure: " + pair.first +
                 " in server: " + pair.second.ip + ":" + std::to_string(pair.second.port) + "\n";
  }

  return mapString;
}

void RoutingMapService::erase(std::pair<std::string, int>& pairToDelete) {
  this->erase({
                  .ip = pairToDelete.first
                  , .port = pairToDelete.second});
}

void RoutingMapService::erase(const ServerRoutingInfo& routingInfo) {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  auto it = _reverseRoutingMap[routingInfo];

  _routingMap.erase(it);
}

RoutingMapIterator RoutingMapService::begin() {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap.begin();
}

RoutingMapIterator RoutingMapService::end() {
  std::lock_guard<std::mutex> lock(_canAccessRoutingMap);

  return _routingMap.end();
}