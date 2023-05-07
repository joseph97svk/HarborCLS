#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "PiecesServer.hpp"

int main (int argc, char** argv) {
    (void) argc;
    (void) argv;

    PiecesServer piecesServer("legoSource.txt", 's', false);
    piecesServer.readLegoSourceFile();

    piecesServer.startServer();

    

    return EXIT_SUCCESS;
}