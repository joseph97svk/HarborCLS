//
// Created by josephvalverde on 12/15/23.
//

#include "Logger.hpp"
#include "CurrentTimeManagement.hpp"
#include <cstring>

static const std::string YELLOW_COLOR = "\033[1;33m";
static const  std::string RED_COLOR = "\033[1;31m";
static const  std::string GREEN_COLOR = "\033[1;32m";
static const  std::string RESET_COLOR = "\033[0m";

static const  std::string LOG_MESSAGE = "Log";
static const  std::string WARNING_MESSAGE = YELLOW_COLOR + "Warning" + RESET_COLOR;
static const  std::string INFO_MESSAGE = GREEN_COLOR + "Info" + RESET_COLOR;
static const  std::string ERROR_MESSAGE = RED_COLOR + "Error" + RESET_COLOR;

namespace HarborCLS {

Logger::Logger (std::shared_ptr<ILoggerBufferingPolicy> bufferingStrategy,
                std::shared_ptr<ILoggerFileManagementPolicy> fileManagementStrategy,
                std::shared_ptr<ILogFileRotation> fileRotationStrategy,
                std::string& logFilePath)
        : _fileManagementStrategy(std::move(fileManagementStrategy))
        , _bufferingStrategy(std::move(bufferingStrategy))
        , _fileRotationStrategy(std::move(fileRotationStrategy))
        , _logFile(_fileManagementStrategy->getLogFile(logFilePath)) {}


void Logger::log(const std::string& message) {
  this->logMessage(LOG_MESSAGE, message);
}

void Logger::warning(const std::string& message) {
  this->logMessage(WARNING_MESSAGE, message);
}

void Logger::error(const std::string& message) {
  this->logMessage(ERROR_MESSAGE, message);
}

void Logger::info(const std::string& message) {
  this->logMessage(INFO_MESSAGE, message);
}

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

void Logger::logMessage(const std::string& messageType, const std::string& message) {
  if (message.empty()) {
    return;
  }

  _loggerThread.Execute([this, message, messageType]() -> void {
    std::string completedMessage = std::string(TimeManager::GetInstance().GetCurrentTime()) + " [ " + messageType + " ] : " + message;;
    const std::optional<std::string> bufferedMessage = _bufferingStrategy->buffer(completedMessage);

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
  });
}
}