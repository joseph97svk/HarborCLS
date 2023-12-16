//
// Created by josephvalverde on 12/11/23.
//

#include <filesystem>
#include "HttpServer.hpp"
#include "JsonReader/json.hpp"
#include "Logger/LoggerFactory/LoggerFactory.hpp"
#include "common.hpp"


HttpServer &HttpServer::getInstance() {
  static HttpServer instance;
  return instance;
}

void HttpServer::addConfiguration(const std::string& configurationJsonPath) {
  if (configurationJsonPath.empty()) {
    _configuration = ServerConfiguration::createDefaultConfiguration();
    return;
  }

  try {
    JsonHandler<ServerConfiguration, ServerConfigurationParsingPolicy> jsonHandler(configurationJsonPath, true);
    _configuration = jsonHandler.deserialize();
  } catch (std::exception& exception) {
    _configuration = ServerConfiguration::createDefaultConfiguration();
  }
}

[[noreturn]] void HttpServer::startServer() {
  this->setUpServer();

  _tcpSocket->listen(_configuration.requestsQueueSize);
  _tcpListener->start();

  _logger->info("Listening on:" + getComputerIp() + ":" + std::to_string(_configuration.port) + "\n");

  for (RequestMiddlewareHandler& handler : _requestMiddlewareHandlers) {
    handler.start();
  }

  for (ApplicationMiddlewareHandler& handler : _applicationMiddlewareHandlers) {
    handler.start();
  }

  for (ResponseMiddlewareHandler& handler : _responseMiddlewareHandlers) {
    handler.start();
  }
}

[[noreturn]] void HttpServer::stopServer() {
  _tcpListener->stop();
}

void HttpServer::setUpServer() {
  LoggerFactory loggerFactory(_configuration.loggerConfiguration);
  _logger = std::move(loggerFactory.createUniqueLogger());

  _logger->info("Starting server");

  std::filesystem::path currentPath = __FILE__;
  currentPath = currentPath.parent_path();

  std::string sslCertPath = currentPath / _configuration.sslCertFileName;
  std::string sslKey = currentPath / _configuration.sslKeyFileName;

  _tcpSocket = std::make_shared<TcpSocket>(14, sslCertPath, sslKey, false);
  _tcpSocket->bind(_configuration.port);

  ListenerMessageBundle messages {
          "Listening for client connections (TCP)\n",
          "request received\n",
          "stopping server\n"
  };

  _tcpListener = std::make_shared<TcpListener>(
          &(_connectionsQueue)
          , _tcpSocket
          , messages
          , nullptr
          , nullptr
          , _configuration.port
  );

  for (int requestHandlerIndex = 0;
       requestHandlerIndex < _configuration.requestHandlerAmount;
       ++requestHandlerIndex) {
    _requestMiddlewareHandlers.emplace_back(
            &(_connectionsQueue)
            , _requestsQueue
            , nullptr
    );
  }

  for (int applicationHandlerIndex = 0;
       applicationHandlerIndex < _configuration.applicationHandlerAmount;
       ++applicationHandlerIndex) {
    _applicationMiddlewareHandlers.emplace_back(
            &(_requestsQueue)
            , _responsesQueue
            , nullptr
    );
  }

  for (int responseHandlerIndex = 0;
       responseHandlerIndex < _configuration.responseHandlerAmount;
       ++responseHandlerIndex) {

    _responseMiddlewareHandlers.emplace_back(
            &(_responsesQueue)
            , nullptr
    );
    _logger->info("Resources initialized");
  }
}


