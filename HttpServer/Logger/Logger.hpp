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
    Logger(std::shared_ptr<ILoggerBufferingPolicy> bufferingStrategy,
           std::shared_ptr<ILoggerFileManagementPolicy> fileManagementStrategy,
           std::shared_ptr<ILogFileRotation> fileRotationStrategy,
           std::string& logFilePath)
            : _bufferingStrategy(std::move(bufferingStrategy))
            , _fileManagementStrategy(std::move(fileManagementStrategy))
            , _fileRotationStrategy(std::move(fileRotationStrategy))
            , _logFile(_fileManagementStrategy->getLogFile(logFilePath)) {}

    ~Logger() override = default;

    void log(std::string message) override  {
      const std::string messageType = "Log";

      this->logMessage(messageType, message);
    }

    void warning(std::string message) override {
      const std::string messageType = "Warning";

      this->logMessage(messageType, message);
    }

    void error(std::string message) override {
      const std::string messageType = "Error";

      this->logMessage(messageType, message);
    }

    void info(std::string message) override {
      const std::string messageType = "Info";

      this->logMessage(messageType, message);
    }

protected:
  template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

  void logMessage(const std::string& messageType, std::string& message) {
    _logFutures.push_back(std::async([this, message, messageType]() {
      std::string completedMessage = getCurrentTime() + " [ " + messageType + " ] : " + message + "\n";
      std::optional<std::string> bufferedMessage = _bufferingStrategy->buffer(completedMessage);

      if (!bufferedMessage.has_value()) return;

      std::optional<std::ofstream> fileStream;

      std::visit(overloaded {
        [&fileStream](std::ofstream& ofs) {
          fileStream = std::move(ofs);
        },
        [&fileStream](const std::string& str) {
          fileStream = std::nullopt;
        }
      }, _logFile);

      _fileManagementStrategy->log(bufferedMessage.value(), fileStream, _canWrite);
    }));
  }
};

#endif //LEGO_FIGURE_MAKER_LOGGER_HPP
