#include "IntermediaryServer.hpp"

#include <csignal>
#include <string>

static std::string certificate = "esjojo.pem";
static std::string key = "key.pem";

void signalHandler (int signal) {
  if (signal == SIGINT) {
    std::cout << "Stopping program\n" << std::endl;

    IntermediaryServer::getInstance(certificate, key).stopServer();
  }
}

int main() {
  // Set signal handlers for SIGINT, SIGTERM and SIGSTOP signals
  signal(SIGINT, signalHandler);

  IntermediaryServer::getInstance(certificate, key);

  IntermediaryServer::getInstance(certificate, key).start();
}

