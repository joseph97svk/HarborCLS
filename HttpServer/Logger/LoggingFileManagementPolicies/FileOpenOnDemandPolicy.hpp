//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_FILEOPENONDEMANDPOLICY_HPP
#define LEGO_FIGURE_MAKER_FILEOPENONDEMANDPOLICY_HPP

#include <iostream>

#include "ILoggerFileManagementPolicy.hpp"
#include "../LogFileRotationPolicies/ILogFileRotation.hpp"

class FileOpenOnDemandPolicy : public ILoggerFileManagementPolicy {
  std::unique_ptr<ILogFileRotation> _logFileRotationPolicy;
  std::string _logFilePath {};

public:
  explicit FileOpenOnDemandPolicy(std::unique_ptr<ILogFileRotation> logFileRotation)
      : _logFileRotationPolicy(std::move(logFileRotation)) {}

  ~FileOpenOnDemandPolicy() override = default;

  std::variant<std::string, std::ofstream> getLogFile(std::string& logFilePath) override {
    _logFilePath = logFilePath;
    return std::move(logFilePath);
  }

  void log(std::string completeLoggingMessage,
           std::optional<std::ofstream>& loggingFile,
           std::mutex& canWrite) override {
    canWrite.lock();

    std::cout << completeLoggingMessage << std::endl;

    std::ofstream fileStream;

    _logFileRotationPolicy->rotateLogFile(fileStream, _logFilePath);
    fileStream << completeLoggingMessage;

    canWrite.unlock();
  }
};

#endif //LEGO_FIGURE_MAKER_FILEOPENONDEMANDPOLICY_HPP
