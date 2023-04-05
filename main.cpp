#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Client.hpp"
using namespace std;

int main() {
    Client socket('s', false);
    socket.getFigure("dragon");
    // Cerrar la conexión
    socket.Close();

    return 0;
}