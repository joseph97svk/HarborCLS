//
// Created by josephvalverde on 12/12/23.
//

#include "../../HttpServer/HttpServer.hpp"
#include "../../HttpServer/JsonReader/JsonHandler.hpp"
#include "../../HttpServer/Logger/LoggerFactory/LoggerFactory.hpp"

#include <filesystem>
#include <iostream>

int main() {
  HttpServer::getInstance();

  std::filesystem::path currentPath = __FILE__;
  currentPath = currentPath.parent_path();
  std::string jsonFile = currentPath / "Configuration.json";

  LoggerFactory loggerFactory;
  std::shared_ptr<ILogger> logger = loggerFactory.createLogger();

  logger->warning(jsonFile);

  JsonHandler<ServerConfiguration, ServerConfigurationParsingPolicy> jsonHandler(jsonFile, true);
  ServerConfiguration configuration = jsonHandler.deserialize();

  return 0;
}