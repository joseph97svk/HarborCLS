//
// Created by josephvalverde on 12/17/23.
//

#ifndef HARBOR_CLS_GENERICWEBAPPLICATION_HPP
#define HARBOR_CLS_GENERICWEBAPPLICATION_HPP

#include <memory>
#include "Server/Http/HttpProtocol.hpp"
#include "Middleware/Listeners/ConnectionListener.hpp"
#include "Middleware/Handlers/ApplicationMiddlewareHandler.hpp"
#include "Middleware/Handlers/RequestMiddlewareHandler.hpp"
#include "../ServerConfiguration.hpp"
#include "../../Common/PathManager.hpp"
#include "../../Logger/LoggerFactory/LoggerFactory.hpp"
#include "../JsonParsingPolicies/ServerConfigurationParsingPolicy.hpp"
#include "../../JsonReader/JsonHandler.hpp"

#include "../Protocols/ProtocolConcept.hpp"

#include "DependencyManagement/Builder.hpp"
#include "DependencyManagement/WebServiceResolver.hpp"
#include "WebAppDefaultServices/WebPageDispatchService.hpp"

#include "Server/WebApplication/WebAppDefaultServices/MVC/Controller/BaseController.hpp"
#include "Server/WebApplication/WebAppDefaultServices/Configuration.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol = HttpProtocol>
  class GenericWebApplication {
  protected:
    using SocketType = typename Protocol::SocketType;
    using RequestType = typename Protocol::RequestType;
    using ResponseType = typename Protocol::ResponseType;

    using RequestParserInterface = IRequestParser<SocketType, RequestType>;

    std::shared_ptr<SocketType> _socket {};
    std::optional<ConnectionListener<SocketType>> _tcpListener {};

    std::vector<RequestMiddlewareHandler<Protocol>> _requestMiddlewareHandlers {};
    std::vector<ApplicationMiddlewareHandler<Protocol>> _applicationMiddlewareHandlers {};

    MiddlewareBlockingQueue<std::shared_ptr<SocketType>> _connectionsQueue {};
    MiddlewareBlockingQueue<std::shared_ptr<RequestType>> _requestsQueue {};
    std::optional<std::reference_wrapper<MiddlewareBlockingQueue<std::shared_ptr<ResponseType>>>> _responsesQueue {};

    std::shared_ptr<IConfiguration<ServerConfiguration>> _configurationService {};

    std::shared_ptr<ILogger> _logger {};

    Builder<Protocol> _dependencyManager {};

    std::shared_ptr<IWebServiceResolver<Protocol>> _webServiceResolver
        = std::make_shared<WebServiceResolver<Protocol>>();

  public:
    GenericWebApplication() {
      defineDirectory();
    }

    NO_COPY(GenericWebApplication)

    /**
     * @brief Sets the default configuration that the server will use if no configuration is provided.
     * @param fallBackConfiguration the default configuration
     */
    void setDefaultFallBackConfiguration(const ServerConfiguration &fallBackConfiguration) {
      _configurationService = std::make_shared<Configuration>(fallBackConfiguration);
    }

    void setCustomWebServiceResolver(std::shared_ptr<IWebServiceResolver<Protocol>> webServiceResolver) {
      _webServiceResolver = webServiceResolver;
    }

    /**
     * Adds a configuration on how the server should behave.
     * @param configurationJsonPath the path of the file that has the configuration.
     */
    void addConfiguration(const std::string &configurationJsonPath) {
      LoggerConfiguration loggerConfiguration{
          .fileAlwaysOpenPolicy = LoggerConfiguration::FileAlwaysOpenPolicy::OPEN_AND_CLOSE,
          .bufferingPolicy = LoggerConfiguration::BufferingPolicy::NO_BUFFER,
          .fileRotationPolicy = LoggerConfiguration::FileRotationPolicy::BOUNDED_ROTATION,
          .sharedLog = false,
          .logFilePath = "Logs/ServerLog.txt",
      };

      LoggerFactory loggerFactory(loggerConfiguration);
      auto tempLogger = loggerFactory.createLogger();

      if (configurationJsonPath.empty()) {
        tempLogger->info("No configuration provided, using default configuration");

        if (_configurationService->getConfiguration() == ServerConfiguration::createDefaultConfiguration()) {
          tempLogger->info("No fallback configuration provided, using default configuration");
        }
      }

      try {
        _configurationService = std::make_shared<Configuration>(configurationJsonPath);
      } catch (std::exception &exception) {
        tempLogger->error("Error while parsing configuration file: " + std::string(exception.what()));
        tempLogger->info("Using default configuration");
        _configurationService = std::make_shared<Configuration>(
            ServerConfiguration::createDefaultConfiguration());
      }

      this->_dependencyManager.addInstance(_configurationService);
    }

    /**
     * Adds a queue that will be used to send responses to the client.
     * @param responsesQueue the queue that will be used to send responses to the client.
     */
    void addResponsesQueue(MiddlewareBlockingQueue<std::shared_ptr<ResponseType>> &responsesQueue) {
      _responsesQueue = responsesQueue;
    }

    /**
     * Initializes the application and starts its operation
     */
    void startApplication(std::shared_ptr<RequestParserInterface> requestParser) {
      bool configurationSet = _configurationService != nullptr;

      if (!configurationSet) {
        _configurationService = std::make_shared<Configuration>(ServerConfiguration::createDefaultConfiguration());
      }

      ServerConfiguration& configuration = _configurationService->getConfiguration();

      LoggerFactory loggerFactory(configuration.loggerConfiguration);
      _logger = loggerFactory.createLogger();

      if (!configurationSet) {
        _logger->warning("No configuration provided nor fallback configuration set. Using default configuration.");
      }

      this->startResources(std::move(requestParser));

      _logger->info("Web application initialized");
      _logger->info("Listening on: " + getComputerIp() + ":" + std::to_string(configuration.port));

      _socket->bind(configuration.port);
      _socket->listen(configuration.requestsQueueSize);

      _tcpListener->start();

      auto& tasksContainer = _dependencyManager.getApplicationStartupTaskSet();
      std::shared_ptr<std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>> livingTasks
          = tasksContainer.getLivingTasks();

      for (auto &livingTask: *livingTasks) {
        livingTask->start();
      }

      for (auto &requestHandler: _requestMiddlewareHandlers) {
        requestHandler.start();
      }

      for (auto &applicationHandler: _applicationMiddlewareHandlers) {
        applicationHandler.start();
      }

      tasksContainer.runInitTasks();
    }

    /**
     * Waits for the application to finish its operation
     * @note This method is blocking
     * @remark This method should be called after startApplication()
     */
    void waitToFinish() {
      _tcpListener->waitToFinish();

      for (auto &requestHandler: _requestMiddlewareHandlers) {
        requestHandler.waitToFinish();
      }

      for (auto &applicationHandler: _applicationMiddlewareHandlers) {
        applicationHandler.waitToFinish();
      }

      _logger->info("Web application resources have successfully been stopped");
    }

    /**
     * Stops the application
     */
    void stopApplication() {
      _tcpListener->stop();
    }

    /**
     * @brief Gives access to the dependency manager to add dependencies and services.
     * @return
     */
    Builder<Protocol>& manageDependencies() {
      return _dependencyManager;
    }

  protected:
    inline void startResources(std::shared_ptr<RequestParserInterface> requestParser) {
      ServerConfiguration& configuration = _configurationService->getConfiguration();

      std::string sslCertPath = configuration.sslCertFileName;
      std::string sslKey = configuration.sslKeyFileName;

      _socket = std::make_shared<SocketType>(false);
      _tcpListener = std::move(std::make_optional<ConnectionListener<SocketType>>(
          _connectionsQueue
          , _socket
          , configuration.port
          , _logger
      ));

      for (unsigned int requestHandlerIndex = 0;
           requestHandlerIndex < configuration.requestHandlerAmount;
           ++requestHandlerIndex) {
        _requestMiddlewareHandlers.emplace_back(
            _connectionsQueue
            , _requestsQueue
            , std::move(requestParser)
            , _logger
        );
      }

      auto& tasksContainer = _dependencyManager.getApplicationStartupTaskSet();
      tasksContainer.initLivingTasks();

      std::shared_ptr<std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>> livingTasks
          = tasksContainer.getLivingTasks();
      for (auto &livingTask: *livingTasks) {
        livingTask->setWebAppLinking(*_responsesQueue, _logger);
      }

      for (unsigned int applicationHandlerIndex = 0;
           applicationHandlerIndex < configuration.applicationHandlerAmount;
           ++applicationHandlerIndex) {
        _applicationMiddlewareHandlers.emplace_back(
            _requestsQueue
            , *_responsesQueue
            , _logger
            , livingTasks
            , _webServiceResolver
        );
      }
    }
  };
}
#endif //HARBOR_CLS_GENERICWEBAPPLICATION_HPP
