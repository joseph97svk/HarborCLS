//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_FILEALWAYSOPENPOLICY_HPP
#define LEGO_FIGURE_MAKER_FILEALWAYSOPENPOLICY_HPP

#include <iostream>

#include "ILoggerFileManagementPolicy.hpp"

class FileAlwaysOpenPolicy : public ILoggerFileManagementPolicy {
  std::unique_ptr<ILogFileRotation> _logFileRotationPolicy;
  std::string _logFilePath {};

public:
  explicit FileAlwaysOpenPolicy(std::unique_ptr<ILogFileRotation> logFileRotation)
      : _logFileRotationPolicy(std::move(logFileRotation)) {}

  ~FileAlwaysOpenPolicy() override = default;

  std::variant<std::string, std::ofstream> getLogFile(std::string& logFilePath) override {
    std::ofstream fileStream;

    this->_logFileRotationPolicy->rotateLogFile(fileStream, logFilePath);

    if (!fileStream.is_open()) {
      throw std::runtime_error("Could not open file: " + logFilePath);
    }

    _logFilePath = logFilePath;

    return std::move(fileStream);
  }

  void log(std::string completeLoggingMessage,
           std::optional<std::ofstream>& loggingFile,
           std::mutex& canWrite) override {
    canWrite.lock();

    auto& fileStream = loggingFile.value();

    std::cout << completeLoggingMessage << std::endl;
    fileStream << completeLoggingMessage;

    canWrite.unlock();
  }
};

#endif //LEGO_FIGURE_MAKER_FILEALWAYSOPENPOLICY_HPP
