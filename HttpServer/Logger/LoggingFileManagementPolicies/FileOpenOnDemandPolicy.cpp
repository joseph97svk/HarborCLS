//
// Created by josephvalverde on 12/15/23.
//

#include "FileOpenOnDemandPolicy.hpp"

FileOpenOnDemandPolicy::FileOpenOnDemandPolicy(std::unique_ptr<ILogFileRotation> logFileRotation)
    : _logFileRotationPolicy(std::move(logFileRotation)) {}

std::variant<std::string, std::ofstream> FileOpenOnDemandPolicy::getLogFile(std::string& logFilePath) {
  _logFilePath = logFilePath;
  return std::move(logFilePath);
}

void FileOpenOnDemandPolicy::log(std::string completeLoggingMessage,
                                 std::optional<std::ofstream>& loggingFile,
                                 std::mutex& canWrite) {
  canWrite.lock();

  std::cout << completeLoggingMessage << std::endl;

  std::ofstream fileStream;

  _logFileRotationPolicy->rotateLogFile(fileStream, _logFilePath);
  fileStream << completeLoggingMessage;

  canWrite.unlock();
}