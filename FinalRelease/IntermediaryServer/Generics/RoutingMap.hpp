#ifndef RoutingMap_HPP
#define RoutingMap_HPP

#include <string>
#include <utility>
#include <unordered_map>
#include <mutex>

class RoutingMap {
 protected:
  std::unordered_map<std::string, std::pair<std::string, int>> myRouteMap;
  std::mutex access;

 public:
  RoutingMap(){

  }
  ~RoutingMap(){
    
  }
  std::pair<std::string, int>& operator[](std::string& figure) {
    this->access.lock();
    std::pair<std::string, int>& pair = myRouteMap[figure];
    this->access.unlock();
    return pair;
  } 

  size_t count(std::string& figure) {
    this->access.lock();
    size_t count = myRouteMap.count(figure);
    this->access.unlock();
    return count;
  }

  void insert(std::pair<std::string, std::pair<std::string, int>> figureStructure) {
    this->access.lock();
    myRouteMap.insert(figureStructure);
    this->access.unlock();
  }

  void erase(std::string& figure) {
    this->access.lock();
    myRouteMap.erase(figure);
    this->access.unlock();
  }

  void showMap() {
    this->access.lock();
    for (auto& pair : myRouteMap) {
      std::cout << pair.first << " -> " << pair.second.first << ", " << pair.second.second << std::endl;
    }
    this->access.unlock();
  }

};

#endif