//
// Created by josephvalverde on 12/11/23.
//

#include <string>

#include "Logger/LoggerConfiguration.hpp"

struct ServerConfiguration {
    unsigned int port{};

    unsigned int requestHandlerAmount{};
    unsigned int applicationHandlerAmount{};
    unsigned int responseHandlerAmount{};
    unsigned int requestsQueueSize{};

    std::string sslCertFileName{};
    std::string sslKeyFileName{};

    LoggerConfiguration loggerConfiguration {
        LoggerConfiguration::FileAlwaysOpenPolicy::OPEN_AND_CLOSE,
        LoggerConfiguration::BufferingPolicy::NO_BUFFER,
        LoggerConfiguration::FileRotationPolicy::BOUNDED_ROTATION,
        true,
        "Logs/log.txt",
        50
    };

    ~ServerConfiguration() = default;

    [[nodiscard]] inline static ServerConfiguration createDefaultConfiguration() {
        return {8080, 1, 1, 1, 16, "", ""};
    }
};