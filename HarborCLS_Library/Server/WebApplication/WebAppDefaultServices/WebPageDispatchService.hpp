//
// Created by josephvalverde on 12/24/23.
//

#ifndef HARBOR_CLS_WEBPAGEDISPATCHSERVICE_HPP
#define HARBOR_CLS_WEBPAGEDISPATCHSERVICE_HPP

#include "../DependencyManagement/BaseWebAppService.hpp"
#include "Server/Http/HttpMessages/Reponse/HttpResponse.hpp"
#include "MVC/Controller/ControllerManager.hpp"
#include "MVC/Controller/BaseController.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class WebServiceDispatchService : public BaseWebAppService<Protocol> {
    using ResponseType = typename Protocol::ResponseType;
    using RequestType = typename Protocol::RequestType;

    std::shared_ptr<ControllerManager<Protocol>> _controllerManager {};

  public:
    explicit WebServiceDispatchService(std::shared_ptr<ControllerManager<Protocol>> controllerManager)
        : BaseWebAppService<Protocol>()
        , _controllerManager(controllerManager) {}

    void release() override {
      // no special loop to be released is present, so nothing to do here
    }

    bool canHandle(std::shared_ptr<RequestType> request) override {
      return false;
    }

    MiddlewareMessage<std::shared_ptr<ResponseType>> handleTask(
        std::shared_ptr<RequestType> request) override {
      if (_controllerManager.get() == nullptr) {
        return MiddlewareMessage<std::shared_ptr<ResponseType>>(
            Error<MessageErrors>(
                "Controller manager could not be resolved. MVC option was not selected for web application."
                , MessageErrors::NON_REGISTERED_SERVICE));
      }

      std::cout << request->header.url << std::endl;

      std::optional<std::shared_ptr<BaseController<Protocol>>> controller
          = _controllerManager->getController(request->header.url);

      if (!controller) {
        return handle404(request);
      }

      std::expected<std::optional<
          std::shared_ptr<ResponseType>>
          , Error<typename BaseController<Protocol>::ControllerError>
          > result =
          controller.value()->processRequest(request, "");

      if (!result) {
        return MiddlewareMessage<std::shared_ptr<ResponseType>>(
            Error<MessageErrors>(
                result.error().what()
                , MessageErrors::CONTROLLER_ERROR));
      }

      if (!result.value()) {
        return handle404(request);
      }

      return MiddlewareMessage<std::shared_ptr<ResponseType>>(result.value().value());
    }

    MiddlewareMessage<std::shared_ptr<ResponseType>> handle404 (std::shared_ptr<RequestType> request) {
      std::shared_ptr<HttpResponse> response = std::make_shared<HttpResponse>();

      response->statusCode = StatusCode::NotFound;
      response->socket = request->socket;

      std::string body = "<html><body><h1>404 Not Found</h1></body></html>";
      response->body = body;
      response->contentLength = body.length();

      return MiddlewareMessage<std::shared_ptr<ResponseType>>(response);
    }
  };
}

#endif //HARBOR_CLS_WEBPAGEDISPATCHSERVICE_HPP
