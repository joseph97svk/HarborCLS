//
// Created by josephvalverde on 1/2/24.
//

#ifndef HARBOR_CLS_HTTPCONTROLLER_HPP
#define HARBOR_CLS_HTTPCONTROLLER_HPP

#include "../../../../WebApplication/WebAppDefaultServices/MVC/Controller/BaseController.hpp"
#include "Server/Http/HttpProtocol.hpp"

namespace HarborCLS {

  class HttpController : public BaseController<HttpProtocol> {
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
    HttpController(const std::string& htmlFileName, std::string layoutFileName)
        : BaseController<HttpProtocol>(htmlFileName, std::move(layoutFileName)) {
    }

    template<typename ControllerImplementation>
    void registerGetHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _getHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerPostHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _postHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerPutHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _putHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerDeleteHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _deleteHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerHeadHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _headHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerTraceHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _traceHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerConnectHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _connectHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    template<typename ControllerImplementation>
    void registerPatchHandler(const std::string& urlDescriptor
        , HandlerFunction<ControllerImplementation> associatedFunction
        , ControllerImplementation& controllerImplementation) {
      _patchHandlers[urlDescriptor] =
          std::make_shared<WebAppHandlerRegistration<HttpRequest, HttpResponse, ControllerImplementation>>(
              std::move(associatedFunction)
              , controllerImplementation);
    }

    std::shared_ptr<HttpResponse> handleOptions(const std::shared_ptr<HttpRequest>& httpRequest) {
      std::shared_ptr<HttpResponse> response = std::make_shared<HttpResponse>();
      response->socket = httpRequest->socket;

      return response;
    }

    std::expected<std::optional<std::shared_ptr<HttpResponse>>, Error<ControllerError>> processRequest(
        const std::shared_ptr<HttpRequest>& httpRequest
        , const std::string& handler) override {
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
          return std::unexpected(Error<ControllerError>("Http MVC Controller: invalid method was given"
              , ControllerError::INVALID_METHOD));
        }
      }

      if (!handlerFunction) {
        return std::unexpected(Error<ControllerError>("Http MVC Controller: invalid handler was given"
            , ControllerError::INVALID_HANDLER));
      }

      std::optional<std::shared_ptr<HttpResponse>> response = handlerFunction->executeHandler(httpRequest);

      return response;
    }

    static std::shared_ptr<HttpResponse> NotFound404Response(std::shared_ptr<HttpRequest> request) {
      std::shared_ptr<HttpResponse> response = std::make_shared<HttpResponse>();
      response->statusCode = StatusCode::NotFound;
      response->body = "404 Not Found";
      response->socket = request->socket;
      response->contentLength = std::get<std::string>(response->body).size();

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

      std::optional<std::string> html = _view->getHtmlContents();

      if (!html) {
        throw std::runtime_error("BaseController: html file could not be read");
      }

      std::shared_ptr<HttpResponse> httpResponse = std::make_shared<HttpResponse>();
      httpResponse->socket = httpRequest->socket;
      httpResponse->body = html.value();
      httpResponse->contentLength = std::get<std::string>(httpResponse->body).size();

      return httpResponse;
    }
  };
}

#endif //HARBOR_CLS_HTTPCONTROLLER_HPP
