#include <string>
#include <iostream>
#include <vector>

#include "Socket.hpp"

class Client : public Socket {
public:
    Client(char type, bool IPv6) : Socket(type, IPv6) {}
    int connectServer();
    bool makeRequest(std::string request);
    void processRequest(bool requestMenu);
    int getFigure(std::string figure);
    bool inAnimalArray(std::string animal);
    int getMenu();
 private:
     std::vector<std::string> animalsArray;
};