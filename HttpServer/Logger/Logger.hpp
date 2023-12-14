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

#include "ILogger.hpp"
#include "LoggingFileManagementStrategy/ILoggerFileManagementStrategy.hpp"
#include "LoggingBufferingStrategies/ILoggingStrategy.hpp"
#include "LoggerConfiguration.hpp"

template <typename LoggingFileManagementStrategy, typename LoggingBufferingStrategy>
class Logger : public ILogger {
    std::unique_ptr<ILoggerFileManagementStrategy> fileManagementStrategy {};
    std::unique_ptr<ILoggingStrategy> bufferingStrategy {};

    std::mutex canWrite {};

    std::variant<std::string, std::ofstream> logFile {};

public:
    Logger(std::string& logFilePath)
            : bufferingStrategy(new LoggingBufferingStrategy())
            , fileManagementStrategy(new LoggingFileManagementStrategy())
    {
      logFile = fileManagementStrategy->getLogFile(logFilePath);
    }

    ~Logger() override = default;

    void log(std::string& message) override  {
      std::optional bufferedMessage = generateMessage("Log", message).value();

      if (!bufferedMessage.has_value()) return;

      std::async(&Logger::log, this->fileManagementStrategy, bufferedMessage.value(), this->logFile);
    }

    void warning(std::string& message) override {
      std::optional bufferedMessage = generateMessage("Warning", message).value();

      if (!bufferedMessage.has_value()) return;

      std::async(&Logger::log, this->fileManagementStrategy, bufferedMessage.value(), this->logFile);
    }

    void error(std::string& message) override {
      std::optional bufferedMessage = generateMessage("Error", message).value();

      if (!bufferedMessage.has_value()) return;

      std::async(&Logger::log, this->fileManagementStrategy, bufferedMessage.value(), this->logFile);
    }

    void info(std::string& message) override {
      std::optional bufferedMessage = generateMessage("Info", message).value();

      if (!bufferedMessage.has_value()) return;

      std::async(&Logger::log, this->fileManagementStrategy, bufferedMessage.value(), this->logFile);
    }

protected:
    std::optional<std::string> generateMessage(std::string& messageType, std::string& message) {
      std::string completedMessage = "[ " + messageType + " ] : " + message + "\n";
      std::optional<std::string> bufferedMessage = bufferingStrategy->buffer(completedMessage);

      if (!bufferedMessage.has_value()) return std::nullopt;

      return bufferedMessage;
    }
};


#endif //LEGO_FIGURE_MAKER_LOGGER_HPP
