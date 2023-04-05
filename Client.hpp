#include <string>
#include <iostream>
#include <vector>

#include "Socket.hpp"

class Client : public Socket {
 private: 
    std::vector<std::string> animalsArray;
 public:
    Client(char type, bool IPv6) : Socket(type, IPv6) {}
    int connectServer();
    bool makeRequest(std::string request);

 private:
    void processRequest(bool requestMenu);
    bool inAnimalArray(std::string animal);
    void regexAnalyzer(bool requestMenu, std::string& line);
    void mainMenuHandle();
    
};