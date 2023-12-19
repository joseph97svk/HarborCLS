//
// Created by josephvalverde on 12/11/23.
//
#ifndef HARBOR_CLS_GENERICSERVER_HPP
#define HARBOR_CLS_GENERICSERVER_HPP

#include <vector>

#include "Middleware/Handlers/ResponseMiddlewareHandler.hpp"
#include "../Logger/Logger.hpp"
#include "WebApplication/WebApplication.hpp"
#include "../Common/common.hpp"
#include "../Common/PathManager.hpp"
#include "Http/HttpBehavior/HttpBehavior.hpp"
#include "Protocols/HttpProtocol.hpp"
#include "../Logger/LoggerFactory/LoggerFactory.hpp"
#include "ServerConfiguration.hpp"

#include "Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol = HttpProtocol>
  class GenericServer {
    using ResponseType = typename Protocol::ResponseType;
    using RequestParserInterface = typename Protocol::RequestParserInterface;
    using RequestParserType = typename Protocol::RequestParserType;
    using ResponseHeaderComposerInterface = typename Protocol::ResponseHeaderComposerInterface;
    using ResponseHeaderComposerType = typename Protocol::ResponseHeaderComposerType;

    std::vector<ResponseMiddlewareHandler> _responseMiddlewareHandlers;
    std::vector<std::reference_wrapper<WebApplication<Protocol>>> _webApplications;
    Queue<std::shared_ptr<ResponseType>> _responsesQueue;

    ServerConfiguration _configuration;

    std::unique_ptr<ILogger> _logger;

    std::shared_ptr<RequestParserInterface> _requestParser;
    std::shared_ptr<ResponseHeaderComposerInterface> _responseHeaderComposer;
  public:
    /*
     * @brief returns the instance of the server
     * @return reference to the server instance
     */
    [[nodiscard]] static GenericServer &getInstance() {
      static GenericServer instance;
      return instance;
    }

    NO_COPY(GenericServer)

    /**
     * @brief adds a web application to the server
     * @param webApplication
     */
    void addWebApplication(WebApplication<Protocol> &webApplication) {
      _webApplications.emplace_back(webApplication);
    }

    /**
     * @brief sets the behaviour on how http requests and responses are handled
     * @param configuration instance of HttpBehavior with the desired behaviour
     */
    void setHttpBehaviour(HttpBehavior<Protocol> &httpBehavior) {
      _requestParser = httpBehavior.requestParser;
      _responseHeaderComposer = httpBehavior.responseHeaderComposer;
    }

    /**
     * Initializes the server and starts the execution of added web applications
     */
    void startServer() {
      LoggerConfiguration loggerConfiguration{
          .fileAlwaysOpenPolicy = LoggerConfiguration::FileAlwaysOpenPolicy::OPEN_AND_CLOSE,
          .bufferingPolicy = LoggerConfiguration::BufferingPolicy::NO_BUFFER,
          .fileRotationPolicy = LoggerConfiguration::FileRotationPolicy::BOUNDED_ROTATION,
          .sharedLog = false,
          .logFilePath = "Logs/ServerLog.txt",
      };

      LoggerFactory loggerFactory(loggerConfiguration);
      _logger = std::move(loggerFactory.createUniqueLogger());

      _logger->info("Starting server");

      this->setUpResources();

      _logger->info("Server resources initialized");

      this->runServerComponents();
    }

    /***
     * Stops the operation of the server
     */
    void stopServer() {
      _logger->info("Stopping server");

      for (WebApplication<Protocol> &webApplication: _webApplications) {
        webApplication.stopApplication();
      }

      _logger->info("Server terminated");
    }

  protected:
    GenericServer() {
      defineDirectory();
    }

    inline void setUpResources() {
      if (_requestParser == nullptr) {
        _requestParser = std::make_shared<RequestParserType>();
      }

      for (WebApplication<Protocol> &webApplication: _webApplications) {
        webApplication.addResponsesQueue(_responsesQueue);
      }

      if (_responseHeaderComposer == nullptr) {
        _responseHeaderComposer = std::make_shared<ResponseHeaderComposerType>();
      }

      for (int responseHandlerIndex = 0;
           responseHandlerIndex < _configuration.responseHandlerAmount;
           ++responseHandlerIndex) {

        _responseMiddlewareHandlers.emplace_back(
            &(_responsesQueue),
            nullptr,
            _responseHeaderComposer
        );
      }
    }

    inline void runServerComponents() {
      for (WebApplication<Protocol> &webApplication: _webApplications) {
        webApplication.startApplication(_requestParser);
      }

      for (ResponseMiddlewareHandler &responseMiddlewareHandler: _responseMiddlewareHandlers) {
        responseMiddlewareHandler.start();
      }

      for (WebApplication<Protocol> &webApplication: _webApplications) {
        webApplication.waitToFinish();
      }

      for (ResponseMiddlewareHandler &responseMiddlewareHandler: _responseMiddlewareHandlers) {
        responseMiddlewareHandler.waitToFinish();
      }
    }
  };

}

#endif //HARBOR_CLS_GENERICSERVER_HPP

