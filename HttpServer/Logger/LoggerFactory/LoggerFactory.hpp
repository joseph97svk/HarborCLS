//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_LOGGERFACTORY_HPP
#define LEGO_FIGURE_MAKER_LOGGERFACTORY_HPP

#include <functional>
#include <unordered_map>

#include <unordered_map>
#include "ILoggerFactory.hpp"
#include "../LoggerConfiguration.hpp"

class LoggerFactory : public ILoggerFactory {
    std::unordered_map<
            std::string,
            std::function<std::shared_ptr<ILogger>()>> loggerCreationMap {};

    LoggerConfiguration loggerConfiguration {};

public:
    LoggerFactory() = default;

    explicit LoggerFactory(LoggerConfiguration loggerConfiguration);

    ~LoggerFactory() override = default;

    std::shared_ptr<ILogger> createLogger() override;

    void registerLoggerCreationFunction(
            std::pair<LoggerConfiguration::FileAlwaysOpenPolicy,
                    LoggerConfiguration::BufferingPolicy> loggerCreationKey,
            std::function<std::shared_ptr<ILogger>()> loggerCreationFunction);

    void registerAllLoggerCreationFunctions();
};


#endif //LEGO_FIGURE_MAKER_LOGGERFACTORY_HPP
