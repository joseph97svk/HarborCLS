//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_ILOGGER_HPP
#define LEGO_FIGURE_MAKER_ILOGGER_HPP

class ILogger {
public:
  virtual ~ILogger() = default;

  /**
  * @brief logs a generic log message
  * @param message message to log
  */
  virtual void log(std::string message) = 0;

  /**
  * @brief logs a warning
  * @param message message to log
  */
  virtual void warning(std::string message) = 0;

  /**
   * @brief logs an error
   * @param message message to log
   */
  virtual void error(std::string message) = 0;

  /**
   * @brief logs an info message
   * @param message message to log
   */
  virtual void info(std::string message) = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGER_HPP
