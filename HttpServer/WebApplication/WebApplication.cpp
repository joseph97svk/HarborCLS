//
// Created by josephvalverde on 12/17/23.
//

#include "WebApplication.hpp"

#include "JsonReader/JsonHandler.hpp"
#include "Logger/LoggerFactory/LoggerFactory.hpp"

void WebApplication::setDefaultFallBackConfiguration(const ServerConfiguration& fallBackConfiguration) {
  _configuration = fallBackConfiguration;
}

void WebApplication::addConfiguration(const std::string &configurationJsonPath) {
  LoggerFactory loggerFactory;
  auto tempLogger = std::move(loggerFactory.createUniqueLogger());

  if (configurationJsonPath.empty()) {
    tempLogger->info("No configuration provided, using default configuration");

    if (_configuration == ServerConfiguration::createDefaultConfiguration()) {
      tempLogger->info("No fallback configuration provided, using default configuration");
    }

    return;
  }

  try {
    JsonHandler<ServerConfiguration, ServerConfigurationParsingPolicy> jsonHandler(configurationJsonPath, true);
    _configuration = jsonHandler.deserialize();
  } catch (std::exception& exception) {
    tempLogger->error("Error while parsing configuration file: " + std::string(exception.what()));
    tempLogger->info("Using default configuration");
    _configuration = ServerConfiguration::createDefaultConfiguration();
  }
}

void WebApplication::addResponsesQueue(BlockingQueue<std::shared_ptr<HttpResponse>> &responsesQueue) {
  _responsesQueue = responsesQueue;
}

void WebApplication::startApplication() {
  _configuration.loggerConfiguration.sharedLog = false;
  LoggerFactory loggerFactory(_configuration.loggerConfiguration);
  _logger = loggerFactory.createUniqueLogger();

  this->startResources();
  _logger->info("Resources initialized");
  _logger->info("Listening on: " + getComputerIp() + ":" + std::to_string(_configuration.port));

  _tcpSocket->bind(_configuration.port);
  _tcpSocket->listen(_configuration.requestsQueueSize);
  _tcpListener->start();

  for (auto& requestHandler : _requestMiddlewareHandlers) {
    requestHandler.start();
  }

  for (auto& applicationHandler : _applicationMiddlewareHandlers) {
    applicationHandler.start();
  }
}

void WebApplication::waitToFinish() {
  _tcpListener->waitToFinish();

  for (auto& requestHandler : _requestMiddlewareHandlers) {
    requestHandler.waitToFinish();
  }

  for (auto& applicationHandler : _applicationMiddlewareHandlers) {
    applicationHandler.waitToFinish();
  }
}

void WebApplication::stopApplication() {
  _logger->info("Stopping server");
  // _tcpListener->stop();
  _tcpSocket->close();
  delete _tcpSocket.get();
  _logger->info("Server terminated");
}

void WebApplication::startResources() {
  ListenerMessageBundle messages {
      "Listening for client connections (TCP)\n",
      "request received\n",
      "stopping server\n"
  };

  std::filesystem::path currentPath = __FILE__;
  currentPath = currentPath.parent_path();

  std::string sslCertPath = currentPath / _configuration.sslCertFileName;
  std::string sslKey = currentPath / _configuration.sslKeyFileName;

  _tcpSocket = std::make_shared<TcpSocket>(false);
  _tcpListener = std::make_optional<TcpListener>(
      &(_connectionsQueue)
      , _tcpSocket
      , messages
      , nullptr
      , nullptr
      , _configuration.port);

  for (int requestHandlerIndex = 0;
       requestHandlerIndex < _configuration.requestHandlerAmount;
       ++requestHandlerIndex) {
    _requestMiddlewareHandlers.emplace_back(
        &_connectionsQueue
        , _requestsQueue
        , nullptr
    );
  }

  for (int applicationHandlerIndex = 0;
       applicationHandlerIndex < _configuration.applicationHandlerAmount;
       ++applicationHandlerIndex) {
    _applicationMiddlewareHandlers.emplace_back(
        &_requestsQueue
        , *_responsesQueue
        , nullptr
    );
  }
}
