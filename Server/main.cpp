#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <csignal>
#include "PiecesServer.hpp"

/**
 * Signal handler function to handle different signals received by the program.
 * @param signal The signal received by the program.
 */
void signal_handler(int signal) {
  if (signal == SIGTERM // kill
      || signal == SIGINT //ctrl + C
      || signal == SIGSTOP // ctrl + Z
      ) {
    std::cout << "\nSTOP PROGRAM" << std::endl;
    PiecesServer::getInstance().stop();
  }
}

/**
 * The main function of the program.
 * @param argc The number of command-line arguments.
 * @param argv The command-line arguments.
 * @return An integer representing the exit status of the program.
 */
int main (int argc, char** argv) {
  (void) argc;
  (void) argv;

  // Set signal handlers for SIGINT, SIGTERM and SIGSTOP signals
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGSTOP, signal_handler);

  // Read Lego source file
  PiecesServer::getInstance().readLegoSourceFile();

  // Read Lego source file named "legoDragonTest.txt"
  PiecesServer::getInstance().readLegoSourceFile("legoDragonTest.txt");

  // Start the server
  PiecesServer::getInstance().startServer();

  return EXIT_SUCCESS; /** Return exit status indicating successful program completion */
}
