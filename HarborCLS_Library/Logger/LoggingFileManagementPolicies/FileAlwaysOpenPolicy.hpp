//
// Created by josephvalverde on 12/14/23.
//

#ifndef HARBOR_CLS_FILEALWAYSOPENPOLICY_HPP
#define HARBOR_CLS_FILEALWAYSOPENPOLICY_HPP

#include <iostream>
#include <memory>
#include <optional>

#include "ILoggerFileManagementPolicy.hpp"
#include "../LogFileRotationPolicies/ILogFileRotation.hpp"

namespace HarborCLS {

  class FileAlwaysOpenPolicy : public ILoggerFileManagementPolicy {
    std::unique_ptr<ILogFileRotation> _logFileRotationPolicy;
    std::string _logFilePath{};

  public:
    /**
     * @brief Constructor
     * @param logFileRotation defines the lifetime of a log file
     */
    explicit FileAlwaysOpenPolicy(std::unique_ptr<ILogFileRotation> logFileRotation);

    /**
     * @brief Destructor
     */
    ~FileAlwaysOpenPolicy() override = default;

    /**
     * @brief Based on the log file rotation policy, returns a log file or the path to a log file
     * @param logFilePath path to the log file
     * @return a log file or the path to a log file
     */
    std::variant<std::string, std::ofstream> getLogFile(std::string &logFilePath) override;

    /**
     * @brief Logs a message to a log file
     * @param completeLoggingMessage message to log
     * @param loggingFile log file
     * @param canWrite mutex to control access to the log file
     */
    void log(std::string completeLoggingMessage,
             std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
             std::mutex &canWrite) override;
  };
}

#endif //HARBOR_CLS_FILEALWAYSOPENPOLICY_HPP
