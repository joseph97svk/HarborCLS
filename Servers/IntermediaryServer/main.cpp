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

  std::string path = std::filesystem::absolute("Configuration.json");

  server.addConfiguration(path);

  server.startServer();

  return 0;
}