#ifndef RoutingMap_HPP
#define RoutingMap_HPP

#include <string>
#include <utility>

class RoutingMap {
    std::pair<std::string, int>& operator[](std::string figure) {
        (void) figure;
        std::string b = "a";
        std::pair<std::string, int>* a = new std::pair<std::string, int>(b, 2);
        return *a;
    } 

	size_t count (std::string figure) {
        (void) figure;
        return 0;
    }

	void insert(std::pair<std::string, int> figureStructure) {
        (void) figureStructure;
    }

	void erase(std::string figure) {
        (void) figure;
    }

};

#endif