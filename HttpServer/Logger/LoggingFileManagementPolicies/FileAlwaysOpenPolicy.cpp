//
// Created by josephvalverde on 12/15/23.
//

#include "FileAlwaysOpenPolicy.hpp"

FileAlwaysOpenPolicy::FileAlwaysOpenPolicy(std::unique_ptr<ILogFileRotation> logFileRotation)
    : _logFileRotationPolicy(std::move(logFileRotation)) {}

std::variant<std::string, std::ofstream> FileAlwaysOpenPolicy::getLogFile(std::string& logFilePath) {
  std::ofstream fileStream;

  this->_logFileRotationPolicy->rotateLogFile(fileStream, logFilePath);

  if (!fileStream.is_open()) {
    throw std::runtime_error("Could not open file: " + logFilePath);
  }

  _logFilePath = logFilePath;

  return std::move(fileStream);
}

void FileAlwaysOpenPolicy::log(std::string completeLoggingMessage,
                               std::optional<std::ofstream>& loggingFile,
                               std::mutex& canWrite) {
  canWrite.lock();

  auto& fileStream = loggingFile.value();

  std::cout << completeLoggingMessage << std::endl;
  fileStream << completeLoggingMessage;

  canWrite.unlock();
}