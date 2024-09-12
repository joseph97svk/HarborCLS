//
// Created by josephvalverde on 12/12/23.
//

#include <iostream>
#include <csignal>

#include <HarborCLS.hpp>

#include "Pages/MainPage.hpp"
#include "Pages/LegoFigurePage.hpp"

#include "../LegoFigureMakerCommon/Services/StartUpPresenceNotificationService.hpp"

#include "Services/LegoDiscoverService.hpp"
#include "Services/RoutingService.hpp"
#include "Services/LegoServerDiscoveryService.hpp"
#include "Services/ImageRequestForwardingService.hpp"

void addServices(HarborCLS::Builder<HarborCLS::HttpProtocol>& services);

int main() {
  HarborCLS::HttpServer& server = HarborCLS::HttpServer::getInstance();
  server.addControlledShutdown(SIGINT, SIGKILL);

  const std::shared_ptr<HarborCLS::HttpWebApplication> intermediaryServer =
      std::make_shared<HarborCLS::HttpWebApplication>();

  server.addWebApplication(intermediaryServer);

  const std::string path = "Servers/IntermediaryServer/Configuration.json";
  intermediaryServer->addConfiguration(path);

  intermediaryServer->addMVC();
  intermediaryServer->addFaviconManager();

  intermediaryServer->addController<MainPage>("");
  intermediaryServer->addController<LegoFigurePage>("legoFigure");

  auto& services = intermediaryServer->manageDependencies();
  addServices(services);

  try {
    server.startServer();
  } catch (std::exception& e) {
    std::cerr << "Server Fatal error - " << e.what() << std::endl;
  }
}

void addServices(HarborCLS::Builder<HarborCLS::HttpProtocol>& services) {
  services.addSingleton<RoutingService>();

  services.addOnStart<LegoDiscoverService>(&LegoDiscoverService::broadcastPresence);

  std::shared_ptr<HarborCLS::BuilderReferenceWrapper<HarborCLS::HttpProtocol>> builderReferenceWrapper =
      std::make_shared<HarborCLS::BuilderReferenceWrapper<HarborCLS::HttpProtocol>>(services);
  services.addInstance(builderReferenceWrapper);

  services.addLivingTask<LegoServerDiscoveryService>();
  services.addLivingTask<HarborCLS::DefaultResourcesProviderService>();
  services.addLivingTask<ImageRequestForwardingService>();

  services.addScoped<StartUpPresenceNotificationService>();
  services.addScoped<FiguresService>();
}

