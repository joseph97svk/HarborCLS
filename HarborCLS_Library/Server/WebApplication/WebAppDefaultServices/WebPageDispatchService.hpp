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
        , _controllerManager(controllerManager) { }

    bool canHandle(std::shared_ptr<RequestType> request) override {
      (void) request;
      return false;
    }

    std::optional<MiddlewareMessage<std::shared_ptr<ResponseType>>> handleTask(
        std::shared_ptr<RequestType> request) override {
      if (_controllerManager.get() == nullptr) {
        return MiddlewareMessage<std::shared_ptr<ResponseType>>(
            Error<MessageErrors>(
                "Controller manager could not be resolved. MVC option was not selected for web application."
                , MessageErrors::NON_REGISTERED_SERVICE));
      }

      std::optional<std::string> controllerUrl = getController(request->header.url);
      if (!controllerUrl) {
        return handle404(request);
      }
      std::optional<std::shared_ptr<BaseController<Protocol>>> controller
          = _controllerManager->getController(*controllerUrl);

      if (!controller) {
        return handle404(request);
      }

      std::optional<std::string> handler = getHandler(request->header.url);
      if (!handler) {
        return handle404(request);
      }

      std::expected<std::optional<
          std::shared_ptr<ResponseType>>
          , Error<typename BaseController<Protocol>::ControllerError>
          > result =
          controller.value()->processRequest(request, *handler);

      if (!result) {
        return MiddlewareMessage<std::shared_ptr<ResponseType>>(
            Error<MessageErrors>(
                result.error().what()
                , MessageErrors::CONTROLLER_ERROR));
      } else if (!result.value()) {
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

  protected:
    static std::optional<std::string> getController(std::string& url) {
      std::string_view controllerUrl = url;
      controllerUrl.remove_prefix(1);

      try {
        if (!controllerUrl.empty()) {
          size_t end = controllerUrl.find('?') == std::string::npos ?
                        controllerUrl.length() : controllerUrl.find('?');
          controllerUrl.remove_suffix(controllerUrl.length() - end);
        }
      } catch (std::exception& e) {
        return std::nullopt;
      }

      return std::string(controllerUrl);
    }

    static std::optional<std::string> getHandler(std::string& url) {
      std::string_view handlerBuffer = url;
      handlerBuffer.remove_prefix(1);

      try {
        if (!handlerBuffer.empty()) {
          handlerBuffer.remove_prefix(handlerBuffer.find('?') + 1);
          size_t end = handlerBuffer.find('=') == std::string::npos ?
                        handlerBuffer.length() : handlerBuffer.find('=');
          handlerBuffer.remove_suffix(handlerBuffer.length() - end);
        }
      } catch (std::exception& e) {
        return std::nullopt;
      }

      return std::string(handlerBuffer);
    }
  };
}

#endif //HARBOR_CLS_WEBPAGEDISPATCHSERVICE_HPP
