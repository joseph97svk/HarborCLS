//
// Created by josephvalverde on 1/12/24.
//

#include "RoutingService.hpp"

using RoutingMapIterator = std::unordered_map<std::string, ServerRoutingInfo>::iterator;

size_t RoutingService::getCount(std::string& figure) {
  return _routingMap.count(figure);
}

void RoutingService::addRoute(std::pair<std::string, ServerRoutingInfo> figureStructure) {
  _routingMap.insert(figureStructure);
}

void RoutingService::erase(std::string& figure) {
  _routingMap.erase(figure);
}

std::string RoutingService::toString() {
  std::string mapString;

  for (auto& pair : _routingMap) {
    mapString += "Figure: " + pair.first +
                 " in server: " + pair.second.ip + ":" + std::to_string(pair.second.port) + "\n";
  }

  return mapString;
}

void RoutingService::erase(std::pair<std::string, int>& pairToDelete) {
  this->erase({
                  .ip = pairToDelete.first
                  , .port = pairToDelete.second});
}

void RoutingService::erase(const ServerRoutingInfo& routingInfo) {
  _routingMap.erase({
                        .ip = routingInfo.ip
                        , .port = routingInfo.port});
}

std::vector<std::string> RoutingService::getAllFigures() {
  std::vector<std::string> figures;

  for (auto& pair : _routingMap) {
    figures.push_back(pair.first);
  }

  return figures;
}

std::optional<ServerRoutingInfo> RoutingService::getRoutingInfo(std::string &figure) {
  if (_routingMap.count(figure) == 0) {
    return std::nullopt;
  }

  return _routingMap[figure];
}
