//
// Created by josephvalverde on 1/4/24.
//

#include <HarborCLS.hpp>

#include "PiecesServer.hpp"
#include "Services/LegoInMemoryRepository.hpp"
#include "Services/LegoPresentService.hpp"

#include "../LegoFigureMakerCommon/Middleware/BroadcastListener.hpp"

int main() {
  PiecesServer& server = PiecesServer::getInstance();

  std::shared_ptr<PiecesWebApplication> webApplication = std::make_shared<PiecesWebApplication>();
  server.addWebApplication(webApplication);

  auto services = webApplication->manageDependencies();

  services.addSingleton<LegoInMemoryRepository>().as<ILegoRepository>();
  services.addOnStart<LegoPresentService>(&LegoPresentService::broadcastPresence);





  return 0;
}