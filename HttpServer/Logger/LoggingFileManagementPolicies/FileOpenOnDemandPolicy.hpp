//
// Created by josephvalverde on 12/14/23.
//

#ifndef LEGO_FIGURE_MAKER_FILEOPENONDEMANDPOLICY_HPP
#define LEGO_FIGURE_MAKER_FILEOPENONDEMANDPOLICY_HPP

#include <iostream>
#include <memory>
#include <optional>

#include "ILoggerFileManagementPolicy.hpp"
#include "../LogFileRotationPolicies/ILogFileRotation.hpp"

class FileOpenOnDemandPolicy : public ILoggerFileManagementPolicy {
  std::unique_ptr<ILogFileRotation> _logFileRotationPolicy;
  std::string _logFilePath {};

public:
  /**
   * @brief Constructor
   * @param logFileRotation defines the lifetime of a log file
   */
  explicit FileOpenOnDemandPolicy(std::unique_ptr<ILogFileRotation> logFileRotation);

  /**
   * @brief Destructor
   */
  ~FileOpenOnDemandPolicy() override = default;

  /**
   * @brief Based on the log file rotation policy, returns a log file or the path to a log file
   * @param logFilePath path to the log file
   * @return a log file or the path to a log file
   */
  std::variant<std::string, std::ofstream> getLogFile(std::string& logFilePath) override;

  /**
   * @brief Logs a message to a log file
   * @param completeLoggingMessage message to log
   * @param loggingFile log file
   * @param canWrite mutex to control access to the log file
   */
  void log(std::string completeLoggingMessage,
           std::optional<std::reference_wrapper<std::ofstream>>& loggingFile,
           std::mutex& canWrite) override;
};

#endif //LEGO_FIGURE_MAKER_FILEOPENONDEMANDPOLICY_HPP
