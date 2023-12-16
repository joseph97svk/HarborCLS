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

#include "../LoggerBuilder/BasicLoggerBuilder.hpp"


template <typename Builder = BasicLoggerBuilder>
class LoggerFactory : public ILoggerFactory {
  LoggerConfiguration _loggerConfiguration {};
  Builder _loggerBuilder {};

public:
  LoggerFactory() = default;

  explicit LoggerFactory(LoggerConfiguration loggerConfiguration)
      : _loggerConfiguration(std::move(loggerConfiguration)) {
  }

  ~LoggerFactory() override = default;

  [[nodiscard]] std::shared_ptr<ILogger> createLogger() override {
    _loggerBuilder.setConfiguration(_loggerConfiguration);

    std::shared_ptr<ILogger> logger = std::move(_loggerBuilder.build());

    _loggerBuilder.reset();
    return logger;
  }

  [[nodiscard]] std::unique_ptr<ILogger> createUniqueLogger() override {
    _loggerBuilder.setConfiguration(_loggerConfiguration);

    std::unique_ptr<ILogger> logger = std::move(_loggerBuilder.build());

    _loggerBuilder.reset();
    return logger;
  }

};


#endif //LEGO_FIGURE_MAKER_LOGGERFACTORY_HPP
