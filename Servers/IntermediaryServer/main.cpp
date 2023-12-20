//
// Created by josephvalverde on 12/12/23.
//

#include "../../HttpServer/HttpServer.hpp"

#include <filesystem>
#include <iostream>
#include <csignal>

void signalHandler(int signal) {
  HttpServer::getInstance().stopServer();
}

int main() {
  signal(SIGINT, signalHandler);

  HttpServer& server = HttpServer::getInstance();

  std::string path = "Servers/IntermediaryServer/Configuration.json";

  WebApplication intermediaryServer;

  intermediaryServer.addConfiguration(path);

  server.addWebApplication(intermediaryServer);

  server.startServer();

  return 0;
}