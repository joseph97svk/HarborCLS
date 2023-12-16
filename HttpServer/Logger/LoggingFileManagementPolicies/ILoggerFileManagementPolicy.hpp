//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_ILOGGERFILEMANEGEMENTSTRATEGY_HPP
#define LEGO_FIGURE_MAKER_ILOGGERFILEMANEGEMENTSTRATEGY_HPP

#include <variant>
#include <string>
#include <fstream>
#include <mutex>

class ILoggerFileManagementPolicy {

public:
  virtual ~ILoggerFileManagementPolicy() = default;

  virtual std::variant<std::string, std::ofstream> getLogFile(std::string& logFilePath) = 0;

  virtual void log(std::string completeLoggingMessage,
                   std::optional<std::ofstream>& loggingFile,
                   std::mutex& canWrite) = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGERFILEMANEGEMENTSTRATEGY_HPP
