//
// Created by josephvalverde on 12/12/23.
//

#include "../../HarborCLS_Library/Server/Http/HttpServer.hpp"

#include <filesystem>
#include <iostream>
#include <csignal>

void signalHandler(int signal) {
  HarborCLS::HttpServer::getInstance().stopServer();
}

int main() {
  signal(SIGINT, signalHandler);
  signal(SIGKILL, signalHandler);

  HarborCLS::HttpServer& server = HarborCLS::HttpServer::getInstance();

  std::string path = "Servers/IntermediaryServer/Configuration.json";

  HarborCLS::WebApplication intermediaryServer;

  intermediaryServer.addConfiguration(path);

  server.addWebApplication(intermediaryServer);

  server.startServer();

  return 0;
}