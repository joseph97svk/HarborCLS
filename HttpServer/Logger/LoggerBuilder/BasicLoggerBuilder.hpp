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
  BasicLoggerBuilder();

  ~BasicLoggerBuilder() override = default;

  void setConfiguration(LoggerConfiguration& loggerConfiguration) override;

  [[nodiscard]] std::unique_ptr<ILogger> build() override;

  void reset() override;

  void registerFilePolicyStrategy(LoggerConfiguration::FileAlwaysOpenPolicy fileAlwaysOpenPolicy,
                                  std::function<std::unique_ptr<ILoggerFileManagementPolicy>()> creationFunction);

  void registerBufferingStrategy(LoggerConfiguration::BufferingPolicy bufferingPolicy,
                                  std::function<std::unique_ptr<ILoggerBufferingPolicy>()> creationFunction);

  void registerFileRotationStrategy(LoggerConfiguration::FileRotationPolicy fileRotationPolicy,
                                     std::function<std::unique_ptr<ILogFileRotation>()> creationFunction);

  void registerAllLoggerCreationFunctions();
};


#endif //LEGO_FIGURE_MAKER_BASICLOGGERBUILDER_HPP
