//
// Created by josephvalverde on 12/12/23.
//

#include <iostream>
#include <csignal>

#include "../../HarborCLS_Library/Server/Http/HttpServer.hpp"
#include "../../HarborCLS_Library/Server/Http/WebApplication/HttpWebApplication.hpp"

#include "Pages/MainPage.hpp"

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

  std::shared_ptr<HarborCLS::HttpWebApplication> intermediaryServer =
      std::make_shared<HarborCLS::HttpWebApplication>();

  auto& services = intermediaryServer->manageDependencies();

  intermediaryServer->addMVC();
  intermediaryServer->addPictureManager();
  intermediaryServer->addFaviconManager();

  intermediaryServer->addController<MainPage>("/");

  services.addOnStart<HelloWorld>(&HelloWorld::sayHello);

  intermediaryServer->addConfiguration(path);

  server.addWebApplication(intermediaryServer);

  try {
    server.startServer();
  } catch (std::exception& e) {
    std::cout << "Server Fatal error - " << e.what() << std::endl;
  }
}