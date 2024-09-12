//
// Created by josephvalverde on 12/11/23.
//
#ifndef HARBOR_CLS_GENERICSERVER_HPP
#define HARBOR_CLS_GENERICSERVER_HPP

#include <vector>
#include <csignal>

#include "Middleware/Handlers/ResponseMiddlewareHandler.hpp"
#include "../Logger/Logger.hpp"
#include "Server/WebApplication/GenericWebApplication.hpp"
#include "../Common/common.hpp"
#include "../Common/PathManager.hpp"
#include "Server/Http/HttpBehavior/ServerBehavior.hpp"
#include "Server/Http/HttpProtocol.hpp"
#include "../Logger/LoggerFactory/LoggerFactory.hpp"
#include "ServerConfiguration.hpp"

#include "Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol = HttpProtocol>
  class GenericServer {
    using ResponseType = typename Protocol::ResponseType;
    using RequestParserType = typename Protocol::RequestParserType;
    using ResponseHeaderComposerType = typename Protocol::ResponseComposerType;
    using ResponseComposerInterface = IResponseComposer<ResponseType>;

    using RequestParserInterface = IRequestParser<typename Protocol::SocketType, typename Protocol::RequestType>;


    std::vector<ResponseMiddlewareHandler<Protocol>> _responseMiddlewareHandlers;
    std::vector<std::shared_ptr<GenericWebApplication<Protocol>>> _webApplications;
    MiddlewareBlockingQueue<std::shared_ptr<ResponseType>> _responsesQueue;

    ServerConfiguration _configuration;

    std::shared_ptr<ILogger> _logger;

    std::shared_ptr<RequestParserInterface> _requestParser;
    std::shared_ptr<ResponseComposerInterface> _responseHeaderComposer;
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
    void addWebApplication(std::shared_ptr<GenericWebApplication<Protocol>> webApplication) {
      _webApplications.emplace_back(webApplication);
    }

    /**
     * @brief sets the behaviour on how http requests and responses are handled
     * @param configuration instance of HttpBehavior with the desired behaviour
     */
    void setBehaviour(ServerBehavior<Protocol> &httpBehavior) {
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
      for (std::shared_ptr<GenericWebApplication<Protocol>>& webApplication : _webApplications) {
        webApplication->stopApplication();
      }
    }

    template<typename... Args>
    void addControlledShutdown(int first, Args... args) {
      const int errnoBuffer = errno;

      signal(first, [](const int signal) {
        (void) signal;
        GenericServer::getInstance().stopServer();
      });

      if constexpr (sizeof...(args) > 0) {
        addControlledShutdown(args...);
      }

      errno = errnoBuffer;
    }

    void addControlledShutdown(int first) {
      const int errnoBuffer = errno;

      signal(first, [](const int signal) {
        (void) signal;
        GenericServer::getInstance().stopServer();
      });

      errno = errnoBuffer;
    }

  protected:
    GenericServer() {
      defineDirectory();
    }

    inline void setUpResources() {
      if (_requestParser == nullptr) {
        _requestParser = std::make_shared<RequestParserType>();
      }

      for (std::shared_ptr<GenericWebApplication<Protocol>>& webApplication : _webApplications) {
        webApplication->addResponsesQueue(_responsesQueue);
      }

      if (_responseHeaderComposer == nullptr) {
        _responseHeaderComposer = std::make_shared<ResponseHeaderComposerType>();
      }

      for (unsigned int responseHandlerIndex = 0;
           responseHandlerIndex < _configuration.responseHandlerAmount;
           ++responseHandlerIndex) {

        _responseMiddlewareHandlers.emplace_back(
            _responsesQueue
            , _responseHeaderComposer
            , _logger
        );
      }
    }

    inline void runServerComponents() {
      for (std::shared_ptr<GenericWebApplication<Protocol>>& webApplication : _webApplications) {
        webApplication->startApplication(_requestParser);
      }

      for (ResponseMiddlewareHandler<Protocol> &responseMiddlewareHandler : _responseMiddlewareHandlers) {
        responseMiddlewareHandler.start();
      }

      for (std::shared_ptr<GenericWebApplication<Protocol>>& webApplication : _webApplications) {
        webApplication->waitToFinish();
      }

      for (ResponseMiddlewareHandler<Protocol> &responseMiddlewareHandler : _responseMiddlewareHandlers) {
        responseMiddlewareHandler.waitToFinish();
      }

      _logger->info("Server operations have successfully been stopped");
    }
  };

}

#endif //HARBOR_CLS_GENERICSERVER_HPP

