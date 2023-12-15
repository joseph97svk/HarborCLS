//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_LOGGERFACTORY_HPP
#define LEGO_FIGURE_MAKER_LOGGERFACTORY_HPP

#include <functional>
#include <unordered_map>
#include <tuple>

#include "ILoggerFactory.hpp"
#include "../LoggerConfiguration.hpp"

#include "../LoggingFileManagementStrategy/FileAlwaysOpenStrategy.hpp"
#include "../LoggingFileManagementStrategy/FileOpenOnDemandStrategy.hpp"

#include "../LoggingBufferingStrategies/ILoggerBufferingStrategy.hpp"
#include "../LogFileRotationStrategy/ILogFileRotation.hpp"

class LoggerFactory : public ILoggerFactory {
    std::unordered_map<
            LoggerConfiguration::FileAlwaysOpenPolicy,
            std::function<ILoggerFileManagementStrategy*()>> fileManagementStrategyCreationFunctions {};

    std::unordered_map<
            LoggerConfiguration::BufferingPolicy,
            std::function<ILoggerBufferingStrategy*()>> bufferingStrategyCreationFunctions {};

    std::unordered_map<
            LoggerConfiguration::FileRotationPolicy,
            std::function<ILogFileRotation*()>> fileRotationStrategyCreationFunctions {};

    LoggerConfiguration loggerConfiguration {};

public:
    LoggerFactory() = default;

    explicit LoggerFactory(LoggerConfiguration loggerConfiguration);

    ~LoggerFactory() override = default;

    [[nodiscard]] std::shared_ptr<ILogger> createLogger() override;

    [[nodiscard]] std::unique_ptr<ILogger> createUniqueLogger() override;

    void registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy fileAlwaysOpenPolicy,
                                    std::function<ILoggerFileManagementStrategy*()> creationFunction);

    void registerBufferingStrategy(LoggerConfiguration::BufferingPolicy bufferingPolicy,
                                   std::function<ILoggerBufferingStrategy*()> creationFunction);

    void registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy fileRotationPolicy,
                                      std::function<ILogFileRotation*()> creationFunction);

    void registerAllLoggerCreationFunctions();

private:
  std::tuple<
          std::shared_ptr<ILoggerFileManagementStrategy>,
          std::shared_ptr<ILoggerBufferingStrategy>,
          std::shared_ptr<ILogFileRotation>> createInjectionComponents();
};


#endif //LEGO_FIGURE_MAKER_LOGGERFACTORY_HPP
