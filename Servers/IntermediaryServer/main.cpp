//
// Created by josephvalverde on 12/12/23.
//

#include <iostream>
#include <csignal>

#include "../../HarborCLS_Library/Server/Http/HttpServer.hpp"

void signalHandler(int signal) {
  HarborCLS::HttpServer::getInstance().stopServer();
}

class HelloWorld {
public:
  void sayHello() {
    std::cout << "Hello World!" << std::endl;
  }
};

int main() {
  signal(SIGINT, signalHandler);
  signal(SIGKILL, signalHandler);

  HarborCLS::HttpServer& server = HarborCLS::HttpServer::getInstance();

  std::string path = "Servers/IntermediaryServer/Configuration.json";

  HarborCLS::GenericWebApplication intermediaryServer;

  intermediaryServer.manageDependencies().addOnStart<HelloWorld>(&HelloWorld::sayHello);

  intermediaryServer.addConfiguration(path);

  server.addWebApplication(intermediaryServer);

  try {
    server.startServer();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }


  return 0;
}