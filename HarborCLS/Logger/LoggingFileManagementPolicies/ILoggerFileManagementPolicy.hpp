//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_ILOGGERFILEMANEGEMENTSTRATEGY_HPP
#define HARBOR_CLS_ILOGGERFILEMANEGEMENTSTRATEGY_HPP

#include <variant>
#include <string>
#include <fstream>
#include <mutex>

namespace HarborCLS {

  class ILoggerFileManagementPolicy {

  public:
    /**
     * @brief virtual destructor
     */
    virtual ~ILoggerFileManagementPolicy() = default;

    /**
     * @brief Based on the log file rotation policy, returns a log file or the path to a log file
     * @param logFilePath path to the log file
     * @return a log file or the path to a log file
     */
    virtual std::variant<std::string, std::ofstream> getLogFile(std::string &logFilePath) = 0;

    /**
     * @brief Logs a message to a log file
     * @param completeLoggingMessage message to log
     * @param loggingFile log file
     * @param canWrite mutex to control access to the log file
     */
    virtual void log(std::string completeLoggingMessage,
                     std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
                     std::mutex &canWrite) = 0;
  };
}

#endif //HARBOR_CLS_ILOGGERFILEMANEGEMENTSTRATEGY_HPP
