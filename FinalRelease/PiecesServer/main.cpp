
#include "Generics/Socket/Socket.hpp"
#include <iostream>

#include "PiecesServer.hpp"

int main () {
    Socket socket('s', false);

    char * osn = (char *) /*"10.1.104.187"*/ "163.178.104.187";
    char * requestChar = (char *) "GET /futbol/2018/ HTTP/1.1\r\nhost: redes.ecci\r\n\r\n";

    socket.Connect(osn, 80);

    std::cout << "Connected" << std::endl;

    std::string request = requestChar;

    socket << request;

    std::string a;

    socket >> a;

    std::cout << a << std::endl;
}