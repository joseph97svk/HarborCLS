//
// Created by josephvalverde on 1/4/24.
//

#include <HarborCLS.hpp>

#include "PiecesServer.hpp"
#include "Services/LegoInMemoryRepository.hpp"
#include "Services/LegoPresentService.hpp"
#include "Services/LegoServerDiscoveryService.hpp"

int main() {
  PiecesServer& server = PiecesServer::getInstance();
  server.addControlledShutdown(SIGINT, SIGKILL);

  std::shared_ptr<PiecesWebApplication> webApplication = std::make_shared<PiecesWebApplication>();
  server.addWebApplication(webApplication);

  auto& services = webApplication->manageDependencies();

  services.addSingleton<LegoInMemoryRepository>().as<ILegoRepository>();

  std::shared_ptr<HarborCLS::BuilderReferenceWrapper<LegoFigureMakerProtocol>> dependencyResolver =
      std::make_shared<HarborCLS::BuilderReferenceWrapper<LegoFigureMakerProtocol>>(services);

  services.addInstance(dependencyResolver);

  services.addOnStart<LegoPresentService>(&LegoPresentService::broadcastPresence);

  services.addScoped<StartUpPresenceNotificationService>();

  services.addLivingTask<LegoServerDiscoveryService>();

  try {
    server.startServer();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}