//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_ILOGGERBUILDER_HPP
#define LEGO_FIGURE_MAKER_ILOGGERBUILDER_HPP

#include <memory>

#include "../LoggerConfiguration.hpp"
#include "../ILogger.hpp"

class ILoggerBuilder {
public:
  virtual ~ILoggerBuilder() = default;

  virtual void setConfiguration(LoggerConfiguration& loggerConfiguration) = 0;

  virtual std::unique_ptr<ILogger> build() = 0;

  virtual void reset() = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGERBUILDER_HPP
