//
// Created by josephvalverde on 12/11/23.
//

#include "HttpServer.hpp"
#include "Logger/LoggerFactory/LoggerFactory.hpp"

HttpServer &HttpServer::getInstance() {
  static HttpServer instance;
  return instance;
}

void HttpServer::addWebApplication(WebApplication &webApplication) {
  _webApplications.emplace_back(webApplication);
}

void HttpServer::startServer() {
  LoggerConfiguration loggerConfiguration {
      .fileAlwaysOpenPolicy = LoggerConfiguration::FileAlwaysOpenPolicy::OPEN_AND_CLOSE,
      .bufferingPolicy = LoggerConfiguration::BufferingPolicy::NO_BUFFER,
      .fileRotationPolicy = LoggerConfiguration::FileRotationPolicy::BOUNDED_ROTATION,
      .sharedLog = false,
      .logFilePath = "Logs/ServerLog.txt",
  };

  LoggerFactory loggerFactory(loggerConfiguration);
  _logger = std::move(loggerFactory.createUniqueLogger());

  _logger->info("Starting server");

  for (WebApplication& webApplication : _webApplications) {
    webApplication.addResponsesQueue(_responsesQueue);
    webApplication.startApplication();
  }

  for (int responseHandlerIndex = 0;
       responseHandlerIndex < _configuration.responseHandlerAmount;
       ++responseHandlerIndex) {

    _responseMiddlewareHandlers.emplace_back(
        &(_responsesQueue), nullptr
    );
  }

  _logger->info("Resources initialized");

  for (ResponseMiddlewareHandler& responseMiddlewareHandler : _responseMiddlewareHandlers) {
    responseMiddlewareHandler.start();
  }

  for (WebApplication& webApplication : _webApplications) {
    webApplication.waitToFinish();
  }

  for (ResponseMiddlewareHandler& responseMiddlewareHandler : _responseMiddlewareHandlers) {
    responseMiddlewareHandler.waitToFinish();
  }
}

void HttpServer::stopServer() {
  _logger->info("Stopping server");

  for (WebApplication& webApplication : _webApplications) {
    webApplication.stopApplication();
  }

  _logger->info("Server terminated");
}



