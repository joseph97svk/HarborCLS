//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_BASICLOGGERBUILDER_HPP
#define HARBOR_CLS_BASICLOGGERBUILDER_HPP

#include <unordered_map>
#include <functional>

#include "ILoggerBuilder.hpp"
#include "../LogFileRotationPolicies/ILogFileRotation.hpp"
#include "../LoggingBufferingPolicies/ILoggerBufferingPolicy.hpp"
#include "../LoggingFileManagementPolicies/ILoggerFileManagementPolicy.hpp"

namespace HarborCLS {

  class BasicLoggerBuilder : public ILoggerBuilder {
  protected:
    std::unordered_map<
        LoggerConfiguration::FileAlwaysOpenPolicy,
        std::function<std::unique_ptr<ILoggerFileManagementPolicy>()>> _fileManagementStrategyCreationFunctions{};

    std::unordered_map<
        LoggerConfiguration::BufferingPolicy,
        std::function<std::unique_ptr<ILoggerBufferingPolicy>()>> _bufferingStrategyCreationFunctions{};

    std::unordered_map<
        LoggerConfiguration::FileRotationPolicy,
        std::function<std::unique_ptr<ILogFileRotation>()>> _fileRotationStrategyCreationFunctions{};

    std::unique_ptr<ILoggerFileManagementPolicy> _fileManagementStrategy{};

    std::unique_ptr<ILoggerBufferingPolicy> _bufferingStrategy{};

    std::unique_ptr<ILogFileRotation> _fileRotationStrategy{};

    LoggerConfiguration _configuration{};

    static std::shared_ptr<ILogger> _uniqueLogger;

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
    void setConfiguration(LoggerConfiguration &loggerConfiguration) override;

    /**
     * @brief Build a logger.
     * @return A logger.
     */
    [[nodiscard]] ILogger* build() override;

    /**
     * @brief Gets the shared instance of the shared logger, if none has been set, it will create a new one with the
     * given configuration
     * @return shared pointer of a logger
     */
    std::shared_ptr<ILogger> getSharedLogger() override;

    /**
     * @brief Sets the unique logger to be shared among all calls to uniqueLogger
     */
    void setUniqueSharedLogger() override;

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
}

#endif //HARBOR_CLS_BASICLOGGERBUILDER_HPP
