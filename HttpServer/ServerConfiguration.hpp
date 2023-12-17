//
// Created by josephvalverde on 12/11/23.
//

#include <string>

#include "Logger/LoggerConfiguration.hpp"

struct ServerConfiguration {
    unsigned int port{8011};

    unsigned int requestHandlerAmount{1};
    unsigned int applicationHandlerAmount{1};
    unsigned int responseHandlerAmount{1};
    unsigned int requestsQueueSize{16};

    std::string sslCertFileName {"defaultSSLCerts/defaultCert.pem"};
    std::string sslKeyFileName {"defaultSSLCerts/defaultKey.pem"};

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
        return {};
    }
};