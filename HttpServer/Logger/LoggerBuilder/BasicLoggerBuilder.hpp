//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_BASICLOGGERBUILDER_HPP
#define LEGO_FIGURE_MAKER_BASICLOGGERBUILDER_HPP

#include <unordered_map>
#include <functional>

#include "ILoggerBuilder.hpp"
#include "../LogFileRotationPolicies/ILogFileRotation.hpp"
#include "../LoggingBufferingPolicies/ILoggerBufferingPolicy.hpp"
#include "../LoggingFileManagementPolicies/ILoggerFileManagementPolicy.hpp"

class BasicLoggerBuilder : public ILoggerBuilder {
  std::unordered_map<
          LoggerConfiguration::FileAlwaysOpenPolicy,
          std::function<std::unique_ptr<ILoggerFileManagementPolicy>()>> _fileManagementStrategyCreationFunctions {};

  std::unordered_map<
          LoggerConfiguration::BufferingPolicy,
          std::function<std::unique_ptr<ILoggerBufferingPolicy>()>> _bufferingStrategyCreationFunctions {};

  std::unordered_map<
          LoggerConfiguration::FileRotationPolicy,
          std::function<std::unique_ptr<ILogFileRotation>()>> _fileRotationStrategyCreationFunctions {};

  std::unique_ptr<ILoggerFileManagementPolicy> _fileManagementStrategy {};

  std::unique_ptr<ILoggerBufferingPolicy> _bufferingStrategy {};

  std::unique_ptr<ILogFileRotation> _fileRotationStrategy {};

  LoggerConfiguration _configuration {};

public:
  /**
   * @brief Create a basic logger builder.
   */
  BasicLoggerBuilder();

  /**
   * @brief Destroy the basic logger builder.
   */
  ~BasicLoggerBuilder() override = default;

  /**
   * @brief Set the logger configuration.
   * @param loggerConfiguration The logger configuration.
   */
  void setConfiguration(LoggerConfiguration& loggerConfiguration) override;

  /**
   * @brief Build a logger.
   * @return A logger.
   */
  [[nodiscard]] std::unique_ptr<ILogger> build() override;

  /**
   * @brief Reset the logger builder.
   */
  void reset() override;

  /**
   * @brief Register a file policy strategy.
   * @param fileAlwaysOpenPolicy The file policy strategy.
   * @param creationFunction The creation function.
   */
  void registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy fileAlwaysOpenPolicy,
                                  std::function<std::unique_ptr<ILoggerFileManagementPolicy>()> creationFunction);

  /**
   * @brief Register a buffering strategy.
   * @param bufferingPolicy The buffering strategy.
   * @param creationFunction The creation function.
   */
  void registerBufferingStrategy(LoggerConfiguration::BufferingPolicy bufferingPolicy,
                                  std::function<std::unique_ptr<ILoggerBufferingPolicy>()> creationFunction);

  /**
   * @brief Register a file rotation strategy.
   * @param fileRotationPolicy The file rotation strategy.
   * @param creationFunction The creation function.
   */
  void registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy fileRotationPolicy,
                                     std::function<std::unique_ptr<ILogFileRotation>()> creationFunction);

  /**
   * @brief Register all file policy creation functions.
   */
  void registerAllLoggerCreationFunctions();
};


#endif //LEGO_FIGURE_MAKER_BASICLOGGERBUILDER_HPP
