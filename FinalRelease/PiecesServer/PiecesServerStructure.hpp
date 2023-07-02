

#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <map>
#include <fstream>
#include <iostream>


struct Lego {
    std::string imageFigure; 
    std::string description; 
    size_t amount;

    Lego(std::string imageFigure = "", std::string description = "", size_t amount = 0) : imageFigure(imageFigure),
    description(description), amount(amount) {}

};

typedef std::map<std::string, std::pair<std::string,
 std::vector<Lego>>> LegoMap;
 