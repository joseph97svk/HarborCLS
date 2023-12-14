//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_FILEALWAYSOPENSTRATEGY_HPP
#define LEGO_FIGURE_MAKER_FILEALWAYSOPENSTRATEGY_HPP

#include <iostream>

#include "ILoggerFileManagementStrategy.hpp"

class FileAlwaysOpenStrategy : public ILoggerFileManagementStrategy {
public:
    FileAlwaysOpenStrategy() = default;

    ~FileAlwaysOpenStrategy() override = default;

    std::variant<std::string, std::ofstream> getLogFile(std::string& logFilePath) override {
      std::ofstream fileStream;
      fileStream.open(logFilePath, std::ios::app);

      return std::move(fileStream);
    }

    void log(std::string &completeLoggingMessage, std::variant<std::string, std::ofstream>& loggingLocation, std::mutex& canWrite) override {
      canWrite.lock();

      auto& fileStream = std::get<std::ofstream>(loggingLocation);

      std::cout << completeLoggingMessage << std::endl;

      fileStream << completeLoggingMessage;

      canWrite.unlock();
    }

};

#endif //LEGO_FIGURE_MAKER_FILEALWAYSOPENSTRATEGY_HPP
