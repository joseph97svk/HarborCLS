#include "IntermediaryServer.hpp"
#include "Generics/RoutingMap.hpp"

#include <csignal>

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
  // signal(SIGTERM, signalHandler);
  // signal(SIGSTOP, signalHandler);

  IntermediaryServer::getInstance(certificate, key);

  IntermediaryServer::getInstance(certificate, key).start();
  // RoutingMap routingM;
  // std::string figure = "Dragon";
 
  // std::pair<std::string, std::pair<std::string, int>> figureStructure(figure,std::make_pair("", 9));
  // routingM.insert(figureStructure);

}