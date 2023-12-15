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
#include "../../common.hpp"

#include "ILogger.hpp"
#include "LoggingFileManagementStrategy/ILoggerFileManagementStrategy.hpp"
#include "LoggingBufferingStrategies/ILoggerBufferingStrategy.hpp"
#include "LogFileRotationStrategy/ILogFileRotation.hpp"
#include "LoggerConfiguration.hpp"

class Logger : public ILogger {
    std::shared_ptr<ILoggerFileManagementStrategy> fileManagementStrategy {};
    std::shared_ptr<ILoggerBufferingStrategy> bufferingStrategy {};
    std::shared_ptr<ILogFileRotation> fileRotationStrategy {};

    std::mutex canWrite {};

    std::variant<std::string, std::ofstream> logFile {};

    std::vector<std::future<void>> logFutures {};

public:
    Logger(std::shared_ptr<ILoggerBufferingStrategy> bufferingStrategy,
           std::shared_ptr<ILoggerFileManagementStrategy> fileManagementStrategy,
           std::shared_ptr<ILogFileRotation> fileRotationStrategy,
           std::string& logFilePath)
            : bufferingStrategy(std::move(bufferingStrategy))
            , fileManagementStrategy(std::move(fileManagementStrategy))
            , fileRotationStrategy(std::move(fileRotationStrategy))
            , logFile(fileManagementStrategy->getLogFile(logFilePath)) {}

    ~Logger() override = default;

    void log(std::string& message) override  {
      std::optional bufferedMessage = generateMessage("Log", message).value();

      if (!bufferedMessage.has_value()) return;

      std::string messageToLog = bufferedMessage.value();
      this->logFutures.push_back(std::async([this, messageToLog]() {
          this->fileManagementStrategy->log(messageToLog, this->logFile, this->canWrite);
      }));
    }

    void warning(std::string& message) override {
      std::optional bufferedMessage = generateMessage("Warning", message).value();

      if (!bufferedMessage.has_value()) return;

      std::string messageToLog = bufferedMessage.value();
      this->logFutures.push_back(std::async([this, messageToLog]() {
          this->fileManagementStrategy->log(messageToLog, this->logFile, this->canWrite);
      }));
    }

    void error(std::string& message) override {
      std::optional bufferedMessage = generateMessage("Error", message).value();

      if (!bufferedMessage.has_value()) return;

      std::string messageToLog = bufferedMessage.value();
      this->logFutures.push_back(std::async([this, messageToLog]() {
          this->fileManagementStrategy->log(messageToLog, this->logFile, this->canWrite);
      }));
    }

    void info(std::string& message) override {
      std::optional bufferedMessage = generateMessage("Info", message).value();

      if (!bufferedMessage.has_value()) return;

      std::string messageToLog = bufferedMessage.value();
      this->logFutures.push_back(std::async([this, messageToLog]() {
          this->fileManagementStrategy->log(messageToLog, this->logFile, this->canWrite);
      }));
    }

protected:
    std::optional<std::string> generateMessage(const std::string& messageType, std::string& message) {
      std::string completedMessage = getCurrentTime() + ": [ " + messageType + " ] : " + message + "\n";
      std::optional<std::string> bufferedMessage = bufferingStrategy->buffer(completedMessage);

      if (!bufferedMessage.has_value()) return std::nullopt;

      return bufferedMessage;
    }
};


#endif //LEGO_FIGURE_MAKER_LOGGER_HPP
