//
// Created by josephvalverde on 12/11/23.
//

#include "HttpServer.hpp"
#include "Logger/LoggerFactory/LoggerFactory.hpp"
#include "Http/ResponseHeaderComposer/ResponseHeaderComposer.hpp"
#include "Http/HttpRequestParser/HttpRequestParser.hpp"

#include "FileManagement/PathManager.hpp"

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

  if (_requestParser == nullptr) {
    _requestParser = std::make_shared<HttpRequestParser>();
  }

  for (WebApplication& webApplication : _webApplications) {
    webApplication.addResponsesQueue(_responsesQueue);
    webApplication.startApplication(_requestParser);
  }

  if (_responseHeaderComposer == nullptr) {
    _responseHeaderComposer = std::make_shared<ResponseHeaderComposer>();
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

  _logger->info("Server resources initialized");

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

void HttpServer::setHttpBehaviour(HttpBehavior &httpBehavior) {
  _requestParser = httpBehavior.requestParser;
  _responseHeaderComposer = httpBehavior.responseHeaderComposer;
}

HttpServer::HttpServer() {
  defineDirectory();
}



