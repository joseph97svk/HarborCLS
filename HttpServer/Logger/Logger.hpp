//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_LOGGER_HPP
#define LEGO_FIGURE_MAKER_LOGGER_HPP

#include <string>
#include <utility>
#include <variant>
#include <fstream>
#include <mutex>
#include <memory>
#include <future>
#include <optional>

#include "../Common/FileManagement.hpp"
#include "../common.hpp"

#include "ILogger.hpp"
#include "LoggingFileManagementPolicies/ILoggerFileManagementPolicy.hpp"
#include "LoggingBufferingPolicies/ILoggerBufferingPolicy.hpp"
#include "LogFileRotationPolicies/ILogFileRotation.hpp"
#include "LoggerConfiguration.hpp"

class Logger : public ILogger {
    std::shared_ptr<ILoggerFileManagementPolicy> _fileManagementStrategy {};
    std::shared_ptr<ILoggerBufferingPolicy> _bufferingStrategy {};
    std::shared_ptr<ILogFileRotation> _fileRotationStrategy {};

    std::mutex _canWrite {};

    std::variant<std::string, std::ofstream> _logFile {};

    std::vector<std::future<void>> _logFutures {};

public:
  /**
   * @brief Construct a new Logger object
   * @param bufferingStrategy how a buffer is managed
   * @param fileManagementStrategy whether a file is always open or opened on demand
   * @param fileRotationStrategy defines the lifetime of a log file
   * @param logFilePath path to the log file
   */
  Logger(std::shared_ptr<ILoggerBufferingPolicy> bufferingStrategy,
         std::shared_ptr<ILoggerFileManagementPolicy> fileManagementStrategy,
         std::shared_ptr<ILogFileRotation> fileRotationStrategy,
         std::string& logFilePath);

  /**
   * @brief Destroy the Logger object
   */
  ~Logger() override = default;

  /**
   * @brief logs a generic log message
   * @param message message to log
   */
  void log(std::string message) override;

  /**
   * @brief logs a warning
   * @param message message to log
   */
  void warning(std::string message) override;

  /**
   * @brief logs an error
   * @param message message to log
   */
  void error(std::string message) override;

  /**
   * @brief logs an info message
   * @param message message to log
   */
  void info(std::string message) override;

protected:
  void logMessage(const std::string& messageType, std::string& message);
};

#endif //LEGO_FIGURE_MAKER_LOGGER_HPP
