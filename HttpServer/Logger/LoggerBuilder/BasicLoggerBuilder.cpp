//
// Created by josephvalverde on 12/15/23.
//

#include <utility>

#include "BasicLoggerBuilder.hpp"
#include "../Logger.hpp"
#include "Logger/LoggingFileManagementPolicies/FileAlwaysOpenPolicy.hpp"
#include "Logger/LogFileRotationPolicies/NoLogRatation.hpp"
#include "Logger/LoggingBufferingPolicies/NoBufferingPolicy.hpp"
#include "Logger/LoggingFileManagementPolicies/FileOpenOnDemandPolicy.hpp"
#include "Logger/LogFileRotationPolicies/BoundedFileRotation.hpp"


BasicLoggerBuilder::BasicLoggerBuilder() {
  this->registerAllLoggerCreationFunctions();
}

void BasicLoggerBuilder::setConfiguration(LoggerConfiguration& loggerConfiguration) {
  _configuration = loggerConfiguration;

  _fileRotationStrategy = std::move(_fileRotationStrategyCreationFunctions[loggerConfiguration.fileRotationPolicy]());
  _fileManagementStrategy = std::move(_fileManagementStrategyCreationFunctions[loggerConfiguration.fileAlwaysOpenPolicy]());
  _bufferingStrategy = std::move(_bufferingStrategyCreationFunctions[loggerConfiguration.bufferingPolicy]());
}


std::unique_ptr<ILogger> BasicLoggerBuilder::build() {
  if (_configuration.sharedLog) {
    static std::unique_ptr<ILogger> uniqueLogger = make_unique<Logger>(
            std::move(_bufferingStrategy),
            std::move(_fileManagementStrategy),
            std::move(_fileRotationStrategy),
            _configuration.logFilePath);

    return std::move(uniqueLogger);
  }

  return make_unique<Logger>(
          std::move(_bufferingStrategy),
          std::move(_fileManagementStrategy),
          std::move(_fileRotationStrategy),
          _configuration.logFilePath);
}

void BasicLoggerBuilder::reset() {
  _fileManagementStrategy = nullptr;
  _bufferingStrategy = nullptr;
  _fileRotationStrategy = nullptr;
  _configuration = LoggerConfiguration();
}

void BasicLoggerBuilder::registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy fileAlwaysOpenPolicy,
                                                    std::function<std::unique_ptr<ILoggerFileManagementPolicy>()> creationFunction) {
  _fileManagementStrategyCreationFunctions[fileAlwaysOpenPolicy] = std::move(creationFunction);
}

void BasicLoggerBuilder::registerBufferingStrategy(LoggerConfiguration::BufferingPolicy bufferingPolicy,
                                                   std::function<std::unique_ptr<ILoggerBufferingPolicy>()> creationFunction) {
  _bufferingStrategyCreationFunctions[bufferingPolicy] = std::move(creationFunction);
}

void BasicLoggerBuilder::registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy fileRotationPolicy,
                                                      std::function<std::unique_ptr<ILogFileRotation>()> creationFunction) {
  _fileRotationStrategyCreationFunctions[fileRotationPolicy] = std::move(creationFunction);
}

void BasicLoggerBuilder::registerAllLoggerCreationFunctions() {
  this->registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy::NO_ROTATION,
                                     []() -> std::unique_ptr<ILogFileRotation> {
                                       return std::make_unique<NoLogFileRotation>();
                                     });

  this->registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy::BOUNDED_ROTATION,
                                     [this]() -> std::unique_ptr<ILogFileRotation> {
                                       return std::make_unique<BoundedFileRotation>(_configuration.maxFileSize);
                                     });

  this->registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy::ALWAYS_OPEN,
                                    [this]() -> std::unique_ptr<ILoggerFileManagementPolicy> {
                                      return std::make_unique<FileAlwaysOpenPolicy>(std::move(_fileRotationStrategy));
                                    });

  this->registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy::OPEN_AND_CLOSE,
                                    [this]() -> std::unique_ptr<ILoggerFileManagementPolicy> {
                                      return std::make_unique<FileOpenOnDemandPolicy>(std::move(_fileRotationStrategy));
                                    });

  this->registerBufferingStrategy(LoggerConfiguration::BufferingPolicy::NO_BUFFER,
                                    []() -> std::unique_ptr<ILoggerBufferingPolicy> {
                                      return std::make_unique<NoBufferingPolicy>();
                                    });
}


