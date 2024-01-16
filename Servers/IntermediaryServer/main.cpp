//
// Created by josephvalverde on 12/12/23.
//

#include <iostream>
#include <csignal>

#include <HarborCLS.hpp>

#include "Pages/MainPage.hpp"

#include "../LegoFigureMakerCommon/Services/StartUpPresenceNotificationService.hpp"

#include "Services/LegoDiscoverService.hpp"
#include "Services/RoutingService.hpp"
#include "Services/LegoServerDiscoveryService.hpp"

int main() {
  HarborCLS::HttpServer& server = HarborCLS::HttpServer::getInstance();

  server.addControlledShutdown(SIGINT, SIGKILL);

  std::string path = "Servers/IntermediaryServer/Configuration.json";

  std::shared_ptr<HarborCLS::HttpWebApplication> intermediaryServer =
      std::make_shared<HarborCLS::HttpWebApplication>();

  auto& services = intermediaryServer->manageDependencies();

  intermediaryServer->addMVC();
  intermediaryServer->addFaviconManager();

  intermediaryServer->addController<MainPage>("/");

  services.addSingleton<RoutingService>();

  services.addOnStart<LegoDiscoverService>(&LegoDiscoverService::broadcastPresence);

  std::shared_ptr<HarborCLS::BuilderReferenceWrapper<HarborCLS::HttpProtocol>> builderReferenceWrapper =
      std::make_shared<HarborCLS::BuilderReferenceWrapper<HarborCLS::HttpProtocol>>(services);

  services.addInstance(builderReferenceWrapper);

  services.addLivingTask<LegoServerDiscoveryService>();
  services.addLivingTask<HarborCLS::DefaultResourcesProviderService>();

  services.addScoped<StartUpPresenceNotificationService>();
  services.addScoped<FiguresService>();

  intermediaryServer->addConfiguration(path);

  server.addWebApplication(intermediaryServer);

  try {
    server.startServer();
  } catch (std::exception& e) {
    std::cout << "Server Fatal error - " << e.what() << std::endl;
  }
}


