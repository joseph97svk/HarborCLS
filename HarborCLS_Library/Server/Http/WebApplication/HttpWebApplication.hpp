//
// Created by josephvalverde on 12/26/23.
//

#ifndef HARBOR_CLS_HTTPWEBAPPLICATION_HPP
#define HARBOR_CLS_HTTPWEBAPPLICATION_HPP

#include "../../WebApplication/GenericWebApplication.hpp"
#include "../HttpProtocol.hpp"

#include "../../WebApplication/WebAppDefaultServices/MVC/Controller/ControllerManager.hpp"
#include "Server/Http/WebApplication/Services/MVC/HttpController.hpp"

namespace HarborCLS {

  class HttpWebApplication : public GenericWebApplication<HttpProtocol> {
    std::shared_ptr<ControllerManager<HttpProtocol>> _controllerManager;

  public:
    HttpWebApplication()
        : GenericWebApplication<HttpProtocol>()
        , _controllerManager(std::make_shared<ControllerManager<HttpProtocol>>(_dependencyManager)) {
    }

    template<typename Controller>
    requires std::is_base_of<HttpController, Controller>::value
    void addController(const std::string controllerName) {
      _controllerManager->addController<Controller>(controllerName);
    }

    void addMVC() {
      _dependencyManager.addInstance<ControllerManager<HttpProtocol>>(_controllerManager);
      _dependencyManager.template addLivingTask<WebServiceDispatchService<HttpProtocol>>();
    }

    void addPictureManager() {

    }

    void addFaviconManager() {

    }
  };

}

#endif //HARBOR_CLS_HTTPWEBAPPLICATION_HPP
