//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_FILEOPENONDEMANDSTRATEGY_HPP
#define LEGO_FIGURE_MAKER_FILEOPENONDEMANDSTRATEGY_HPP

#include <iostream>

#include "ILoggerFileManagementStrategy.hpp"

class FileOpenOnDemandStrategy : public ILoggerFileManagementStrategy {
public:
    FileOpenOnDemandStrategy() = default;

    ~FileOpenOnDemandStrategy() override = default;

    std::variant<std::string, std::ofstream> getLogFile(std::string& logFilePath) override {
      return std::move(logFilePath);
    }

    void log(std::string &completeLoggingMessage, std::variant<std::string, std::ofstream>& loggingLocation, std::mutex& canWrite) override {
      auto& logFilePath = std::get<std::string>(loggingLocation);

      canWrite.lock();

      std::cout << completeLoggingMessage << std::endl;

      std::ofstream fileStream;
      fileStream.open(logFilePath, std::ios::app);
      fileStream << completeLoggingMessage;

      canWrite.unlock();
    }
};

#endif //LEGO_FIGURE_MAKER_FILEOPENONDEMANDSTRATEGY_HPP
