//
// Created by josephvalverde on 12/25/23.
//

#ifndef HARBOR_CLS_BASECONTROLLER_HPP
#define HARBOR_CLS_BASECONTROLLER_HPP

#include <string>
#include <unordered_map>
#include <functional>
#include <utility>

#include "Server/Http/HttpMessages/Request/HttpRequest.hpp"
#include "Server/Http/HttpMessages/Reponse/HttpResponse.hpp"
#include "Server/WebApplication/WebAppDefaultServices/MVC/View/BaseView.hpp"
#include "WebAppHandlerRegistration.hpp"

namespace HarborCLS {

  class BaseController {
  public:
    using HandlerFunction = std::function<std::optional<std::shared_ptr<HttpResponse>>(std::shared_ptr<HttpRequest>)>;

    enum class HttpControllerError {
      GENERIC_ERROR,
      INVALID_HANDLER,
      INVALID_METHOD
    };

  protected:
    std::optional<BaseView> _view {};

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _getHandlers {};

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _postHandlers {};

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _putHandlers {};

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _deleteHandlers {};

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _headHandlers {};

    // options handled by server on default

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _traceHandlers {};

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _connectHandlers {};

    std::unordered_map<
        std::string
        , std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>>
        > _patchHandlers {};

  public:
    explicit BaseController(const std::string& htmlFileName, std::string layoutFileName) noexcept(false){

      if (htmlFileName.empty()) {
        throw std::runtime_error("BaseController: html file name cannot be empty");
      }

      std::string extension = std::filesystem::path(htmlFileName).extension().string();

      if (extension != ".html" && extension != ".htm") {
        throw std::runtime_error("BaseController: html file must have .html or .htm extension");
      }

      if (!std::filesystem::exists(htmlFileName)) {
        throw std::runtime_error("BaseController: html file does not exist");
      }

      _view = BaseView(htmlFileName, layoutFileName);
    }

    virtual ~BaseController() = default;

    template<typename ControllerImplementation>
    void registerGetHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _getHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerPostHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _postHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerPutHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _putHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerDeleteHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _deleteHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerHeadHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _headHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerTraceHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _traceHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerConnectHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _connectHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerPatchHandler(const std::string& urlDescriptor
                         , HandlerFunction associatedFunction
                         , ControllerImplementation& controllerImplementation) {
      _patchHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    std::shared_ptr<HttpResponse> handleOptions(const std::shared_ptr<HttpRequest>& httpRequest) {

    }

    std::expected<std::optional<std::shared_ptr<HttpResponse>>, Error<HttpControllerError>> processRequest(
        const std::shared_ptr<HttpRequest>& httpRequest
        , const std::string& handler) {
      HttpHeader& header = httpRequest->header;

      if (header.method == HttpMappings::Method::GET) {
        return this->handleGetRequest(httpRequest, handler);
      }

      if (header.method == HttpMappings::Method::OPTIONS) {
        return this->handleOptions(httpRequest);
      }

      std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>> handlerFunction;

      switch (header.method) {
        case HttpMappings::Method::POST: {
          handlerFunction = _postHandlers[handler];
          break;
        }
        case HttpMappings::Method::PUT: {
          handlerFunction = _putHandlers[handler];
          break;
        }
        case HttpMappings::Method::DELETE: {
          handlerFunction = _deleteHandlers[handler];
          break;
        }
        case HttpMappings::Method::HEAD: {
          handlerFunction = _headHandlers[handler];
          break;
        }
        case HttpMappings::Method::TRACE: {
          handlerFunction = _traceHandlers[handler];
          break;
        }
        case HttpMappings::Method::CONNECT: {
          handlerFunction = _connectHandlers[handler];
          break;
        }
        case HttpMappings::Method::PATCH: {
          handlerFunction = _patchHandlers[handler];
          break;
        }
        default: {
          return std::unexpected(Error<HttpControllerError>("Http MVC Controller: invalid method was given"
                                                            , HttpControllerError::INVALID_METHOD));
        }
      }

      if (!handlerFunction) {
        return std::unexpected(Error<HttpControllerError>("Http MVC Controller: invalid handler was given"
                                                          , HttpControllerError::INVALID_HANDLER));
      }

      std::optional<std::shared_ptr<HttpResponse>> response = handlerFunction->executeHandler(httpRequest);

      return response;
    }

  private:
    std::optional<std::shared_ptr<HttpResponse>> handleGetRequest(
        const std::shared_ptr<HttpRequest>& httpRequest
        , const std::string& handler) {
      std::shared_ptr<IWebAppHandlerRegistration<HttpRequest, HttpResponse>> associatedFunction
          = _getHandlers[handler];

      if (associatedFunction) {
        std::optional<std::shared_ptr<HttpResponse>> response = associatedFunction->executeHandler(httpRequest);

        if (response) {
          return response.value();
        }
      }

      std::optional<std::string> html = std::move(_view->getHtmlContents());

      if (!html) {
        throw std::runtime_error("BaseController: html file could not be read");
      }

      std::shared_ptr<HttpResponse> httpResponse = std::make_shared<HttpResponse>();
      httpResponse->socket = httpRequest->socket;
      httpResponse->body = html.value();

      return httpResponse;
    }
  };
}

#endif //HARBOR_CLS_BASECONTROLLER_HPP
