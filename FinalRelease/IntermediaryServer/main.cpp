#include "IntermediaryServer.hpp"

#include <csignal>

static std::string certificate = "esjojo.pem";
static std::string key = "key.pem";

void signalHandler (int signal) {
  std::cout << "Stopping program" << std::endl;

  IntermediaryServer::getInstance(certificate, key).stopServer();
}

int main() {
  // Set signal handlers for SIGINT, SIGTERM and SIGSTOP signals
  signal(SIGINT, signalHandler);
  // signal(SIGTERM, signalHandler);
  // signal(SIGSTOP, signalHandler);

  IntermediaryServer::getInstance(certificate, key);

  IntermediaryServer::getInstance(certificate, key).start();
}