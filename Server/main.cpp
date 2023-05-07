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
  if (signal == SIGTERM // kill
      || signal == SIGINT //ctrl + C
      || signal == SIGSTOP // ctrl + Z
      ) {
    std::cout << "\nSTOP PROGRAM" << std::endl;
    PiecesServer::getInstance().stop();
  }
}

int main (int argc, char** argv) {
  (void) argc;
  (void) argv;
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGSTOP, signal_handler);

  PiecesServer::getInstance().readLegoSourceFile();
  PiecesServer::getInstance().startServer();
  return EXIT_SUCCESS;
}