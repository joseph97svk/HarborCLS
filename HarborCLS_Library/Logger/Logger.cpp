//
// Created by josephvalverde on 12/15/23.
//

#include "Logger.hpp"

#include <cassert>

namespace HarborCLS {

Logger::Logger (std::shared_ptr<ILoggerBufferingPolicy> bufferingStrategy,
                std::shared_ptr<ILoggerFileManagementPolicy> fileManagementStrategy,
                std::shared_ptr<ILogFileRotation> fileRotationStrategy,
                std::string& logFilePath)
        : _fileManagementStrategy(std::move(fileManagementStrategy))
        , _bufferingStrategy(std::move(bufferingStrategy))
        , _fileRotationStrategy(std::move(fileRotationStrategy))
        , _logFile(_fileManagementStrategy->getLogFile(logFilePath)) {}


void Logger::log(std::string message) {
  const std::string messageType = "Log";

  this->logMessage(messageType, message);
}

void Logger::warning(std::string message) {
  const std::string yellowColor = "\033[1;33m";
  const std::string resetColor = "\033[0m";
  const std::string messageType = yellowColor + "Warning" + resetColor;

  this->logMessage(messageType, message);
}

void Logger::error(std::string message) {
  const std::string redColor = "\033[1;31m";
  const std::string resetColor = "\033[0m";
  const std::string messageType = redColor + "Error" + resetColor;

  this->logMessage(messageType, message);
}

void Logger::info(std::string message) {
  const std::string greenColor = "\033[1;32m";
  const std::string resetColor = "\033[0m";
  const std::string messageType = greenColor + "Info" + resetColor;

  this->logMessage(messageType, message);
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

void Logger::logMessage(const std::string& messageType, std::string& message) {
  assert(!message.empty());

  while (!_logFutures.empty()) {
    _logFutures.pop_back();
  }

  _logFutures.push_back(std::async([this, message, messageType]() -> void {
    std::string completedMessage = getCurrentTime() + " [ " + messageType + " ] : " + message;
    std::optional<std::string> bufferedMessage = _bufferingStrategy->buffer(completedMessage);

    if (!bufferedMessage.has_value()) return;

    std::optional<std::reference_wrapper<std::ofstream>> fileStream;

    std::visit(overloaded {
      [&fileStream](std::ofstream& ofs) {
        fileStream = ofs;
      },
      [&fileStream](const std::string& str) {
        (void) str;
        fileStream = std::nullopt;
      }
    }, _logFile);

    _fileManagementStrategy->log(bufferedMessage.value(), fileStream, _canWrite);
  }));
}
}