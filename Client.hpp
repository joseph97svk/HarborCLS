#include <string>
#include <iostream>

#include "Socket.hpp"

class Client : public Socket {
public:
    Client(char type, bool IPv6) : Socket(type, IPv6) {}
    int getFigure(std::string figure);
};