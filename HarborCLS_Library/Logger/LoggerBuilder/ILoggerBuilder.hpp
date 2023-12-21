//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_ILOGGERBUILDER_HPP
#define HARBOR_CLS_ILOGGERBUILDER_HPP

#include <memory>

#include "../LoggerConfiguration.hpp"
#include "../ILogger.hpp"

namespace HarborCLS {

  class ILoggerBuilder {
  public:
    virtual ~ILoggerBuilder() = default;

    /**
     * @brief Set the logger configuration.
     * @param loggerConfiguration The logger configuration.
     */
    virtual void setConfiguration(LoggerConfiguration &loggerConfiguration) = 0;

    /**
     * @brief Build a logger.
     * @remark whomever calls this function is responsible for taking ownership of the logger
     * That is an expected behavior from the built in factory
     * @return pointer of a logger
     */
    virtual ILogger* build() = 0;

    /**
     * @brief Reset the logger builder.
     */
    virtual void reset() = 0;
  };
}

#endif //HARBOR_CLS_ILOGGERBUILDER_HPP
