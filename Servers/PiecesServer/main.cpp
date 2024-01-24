//
// Created by josephvalverde on 1/4/24.
//

#include <HarborCLS.hpp>

#include "PiecesServer.hpp"
#include "Services/LegoInMemoryRepository.hpp"
#include "Services/LegoPresentService.hpp"
#include "Services/LegoServerDiscoveryService.hpp"
#include "Services/FigureRetrievalService.hpp"
#include "Services/FigureImageRetrievalService.hpp"

void addServices(HarborCLS::Builder<LegoFigureMakerProtocol>& services);

int main() {
  PiecesServer& server = PiecesServer::getInstance();
  server.addControlledShutdown(SIGINT, SIGKILL);

  std::string configurationPath = "Servers/PiecesServer/Configuration.json";

  std::shared_ptr<PiecesWebApplication> webApplication = std::make_shared<PiecesWebApplication>();
  server.addWebApplication(webApplication);
  webApplication->addConfiguration(configurationPath);

  auto& services = webApplication->manageDependencies();
  addServices(services);

  try {
    server.startServer();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}

void addServices(HarborCLS::Builder<LegoFigureMakerProtocol>& services) {
  services.addSingleton<LegoInMemoryRepository>().as<ILegoRepository>();

  std::shared_ptr<HarborCLS::BuilderReferenceWrapper<LegoFigureMakerProtocol>> dependencyResolver =
      std::make_shared<HarborCLS::BuilderReferenceWrapper<LegoFigureMakerProtocol>>(services);
  services.addInstance(dependencyResolver);

  services.addOnStart<LegoPresentService>(&LegoPresentService::broadcastPresence);

  services.addScoped<StartUpPresenceNotificationService>();

  services.addLivingTask<LegoServerDiscoveryService>();
  services.addLivingTask<FigureRetrievalService>();
  services.addLivingTask<FigureImageRetrievalService>();
}