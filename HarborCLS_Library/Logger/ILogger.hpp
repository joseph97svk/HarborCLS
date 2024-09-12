//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_ILOGGER_HPP
#define HARBOR_CLS_ILOGGER_HPP

namespace HarborCLS {

  class ILogger {
  public:
    virtual ~ILogger() = default;

    /**
    * @brief logs a generic log message
    * @param message message to log
    */
    virtual void log(const std::string& message) = 0;

    /**
    * @brief logs a warning
    * @param message message to log
    */
    virtual void warning(const std::string& message) = 0;

    /**
     * @brief logs an error
     * @param message message to log
     */
    virtual void error(const std::string& message) = 0;

    /**
     * @brief logs an info message
     * @param message message to log
     */
    virtual void info(const std::string& message) = 0;
  };
}
#endif //HARBOR_CLS_ILOGGER_HPP
