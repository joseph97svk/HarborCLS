#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include "PiecesServer.hpp"



void signal_handler(int signal) {
  (void)signal;
  if (signal == SIGTERM || signal == SIGINT) {
    std::cout << "\nSTOP PROGRAM" << std::endl;
    PiecesServer::getInstance().stop();
  }
}


int main (int argc, char** argv) {
    (void) argc;
    (void) argv;
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    PiecesServer::getInstance().readLegoSourceFile();
    PiecesServer::getInstance().startServer();
    std::cout<<"termina programa" << std::endl;
    return EXIT_SUCCESS;
}