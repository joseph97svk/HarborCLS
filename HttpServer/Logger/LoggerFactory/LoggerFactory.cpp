//
// Created by josephvalverde on 12/14/23.
//

#include <memory>

#include "LoggerFactory.hpp"
#include "../Logger.hpp"
#include "../LoggingBufferingStrategies/NoBufferingStrategy.hpp"
#include "../LoggingFileManagementStrategy/FileAlwaysOpenStrategy.hpp"
#include "../LoggingFileManagementStrategy/FileOpenOnDemandStrategy.hpp"


LoggerFactory::LoggerFactory(LoggerConfiguration loggerConfiguration)
    : loggerConfiguration(loggerConfiguration) {
  registerAllLoggerCreationFunctions();
}

std::shared_ptr<ILogger> LoggerFactory::createLogger() {
  return std::shared_ptr<ILogger>(new Logger<FileOpenOnDemandStrategy, NoBufferingStrategy>(
      this->loggerConfiguration.logFilePath));
  registerAllLoggerCreationFunctions();
}

void LoggerFactory::registerLoggerCreationFunction(std::pair<LoggerConfiguration::FileAlwaysOpenPolicy,
        LoggerConfiguration::BufferingPolicy> loggerCreationKey,
                                                   std::function<std::shared_ptr<ILogger>()> loggerCreationFunction) {
  std::string key = std::to_string(static_cast<int>(loggerCreationKey.first)) +
                    std::to_string(static_cast<int>(loggerCreationKey.second));

  this->loggerCreationMap[key] = std::move(loggerCreationFunction);
}

void LoggerFactory::registerAllLoggerCreationFunctions() {
  this->registerLoggerCreationFunction(
      std::make_pair(LoggerConfiguration::FileAlwaysOpenPolicy::DEFAULT,
                     LoggerConfiguration::BufferingPolicy::DEFAULT),
      [this]() -> std::shared_ptr<ILogger> {
        return std::shared_ptr<ILogger>(new Logger<FileAlwaysOpenStrategy, NoBufferingStrategy>(
            this->loggerConfiguration.logFilePath));
      });

  this->registerLoggerCreationFunction(
      std::make_pair(LoggerConfiguration::FileAlwaysOpenPolicy::DEFAULT,
                     LoggerConfiguration::BufferingPolicy::NO_BUFFER),
      [this]() -> std::shared_ptr<ILogger> {
        return std::shared_ptr<ILogger>(new Logger<FileAlwaysOpenStrategy, NoBufferingStrategy>(
            this->loggerConfiguration.logFilePath));
      });

  this->registerLoggerCreationFunction(
      std::make_pair(LoggerConfiguration::FileAlwaysOpenPolicy::ALWAYS_OPEN,
                     LoggerConfiguration::BufferingPolicy::DEFAULT),
      [this]() -> std::shared_ptr<ILogger> {
        return std::shared_ptr<ILogger>(new Logger<FileAlwaysOpenStrategy, NoBufferingStrategy>(
            this->loggerConfiguration.logFilePath));
      });

  this->registerLoggerCreationFunction(
      std::make_pair(LoggerConfiguration::FileAlwaysOpenPolicy::ALWAYS_OPEN,
                     LoggerConfiguration::BufferingPolicy::NO_BUFFER),
      [this]() -> std::shared_ptr<ILogger> {
        return std::shared_ptr<ILogger>(new Logger<FileAlwaysOpenStrategy, NoBufferingStrategy>(
            this->loggerConfiguration.logFilePath));
      });
}
