//
// Created by josephvalverde on 12/14/23.
//

#include <memory>

#include "LoggerFactory.hpp"
#include "../Logger.hpp"
#include "../LoggingBufferingStrategies/NoBufferingStrategy.hpp"
#include "../LogFileRotationStrategy/NoLogRatation.hpp"
#include "../LogFileRotationStrategy/BoundedFileRotation.hpp"

LoggerFactory::LoggerFactory(LoggerConfiguration loggerConfiguration)
    : loggerConfiguration(loggerConfiguration) {
  registerAllLoggerCreationFunctions();
}

std::shared_ptr<ILogger> LoggerFactory::createLogger() {
  auto [fileManagementStrategy, bufferingStrategy, fileRotationStrategy] = createInjectionComponents();

  if (this->loggerConfiguration.sharedLog) {
    static std::shared_ptr<ILogger> sharedLogger = std::make_shared<Logger>(
            bufferingStrategy,
            fileManagementStrategy,
            fileRotationStrategy,
            this->loggerConfiguration.logFilePath);
    return sharedLogger;
  }

  return std::make_shared<Logger>(
          bufferingStrategy,
          fileManagementStrategy,
          fileRotationStrategy,
          this->loggerConfiguration.logFilePath);
}

std::unique_ptr<ILogger> LoggerFactory::createUniqueLogger() {
  auto [fileManagementStrategy, bufferingStrategy, fileRotationStrategy] = createInjectionComponents();

  if (this->loggerConfiguration.sharedLog) {
    static std::unique_ptr<ILogger> uniqueLogger = std::make_unique<Logger>(
            bufferingStrategy,
            fileManagementStrategy,
            fileRotationStrategy,
            this->loggerConfiguration.logFilePath);
    return std::move(uniqueLogger);
  }

  return std::make_unique<Logger>(
          bufferingStrategy,
          fileManagementStrategy,
          fileRotationStrategy,
          this->loggerConfiguration.logFilePath);
}

void LoggerFactory::registerAllLoggerCreationFunctions() {
    this->registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy::NO_ROTATION,
                                       []() -> ILogFileRotation* {
                                           return new NoLogFileRotation();
                                       });

    this->registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy::BOUNDED_ROTATION,
                                       [this]() -> ILogFileRotation* {
                                           return new BoundedFileRotation(this->loggerConfiguration.maxFileSize);
                                       });

    this->registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy::ALWAYS_OPEN,
                                      []() -> ILoggerFileManagementStrategy* {
                                          return new FileAlwaysOpenStrategy();
                                      });

    this->registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy::OPEN_AND_CLOSE,
                                      []() -> ILoggerFileManagementStrategy* {
                                          return new FileOpenOnDemandStrategy();
                                      });

    this->registerBufferingStrategy(LoggerConfiguration::BufferingPolicy::NO_BUFFER,
                                      []() -> ILoggerBufferingStrategy* {
                                          return new NoBufferingStrategy();
                                      });
}

void LoggerFactory::registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy fileAlwaysOpenPolicy,
                                               std::function<ILoggerFileManagementStrategy*()> creationFunction) {
    this->fileManagementStrategyCreationFunctions[fileAlwaysOpenPolicy] = std::move(creationFunction);
}

void LoggerFactory::registerBufferingStrategy(LoggerConfiguration::BufferingPolicy bufferingPolicy,
                                              std::function<ILoggerBufferingStrategy*()> creationFunction) {
    this->bufferingStrategyCreationFunctions[bufferingPolicy] = std::move(creationFunction);
}

void LoggerFactory::registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy fileRotationPolicy,
                                                 std::function<ILogFileRotation*()> creationFunction) {
    this->fileRotationStrategyCreationFunctions[fileRotationPolicy] = std::move(creationFunction);
}

std::tuple<
        std::shared_ptr<ILoggerFileManagementStrategy>,
        std::shared_ptr<ILoggerBufferingStrategy>,
        std::shared_ptr<ILogFileRotation>> LoggerFactory::createInjectionComponents() {
  auto fileManagementStrategy =
          std::shared_ptr<ILoggerFileManagementStrategy>
                  (this->fileManagementStrategyCreationFunctions[this->loggerConfiguration.fileAlwaysOpenPolicy]());
  auto bufferingStrategy =
          std::shared_ptr<ILoggerBufferingStrategy>
                  (this->bufferingStrategyCreationFunctions[this->loggerConfiguration.bufferingPolicy]());
  auto fileRotationStrategy =
          std::shared_ptr<ILogFileRotation>
                  (this->fileRotationStrategyCreationFunctions[this->loggerConfiguration.fileRotationPolicy]());

  return std::make_tuple(fileManagementStrategy, bufferingStrategy, fileRotationStrategy);
}