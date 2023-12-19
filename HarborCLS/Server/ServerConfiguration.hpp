//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_SERVERCONFIGURATION_HPP
#define HARBOR_CLS_SERVERCONFIGURATION_HPP

#include <string>
namespace HarborCLS {
  struct ServerConfiguration {
    unsigned int port{8017};

    unsigned int requestHandlerAmount{1};
    unsigned int applicationHandlerAmount{1};
    unsigned int responseHandlerAmount{1};
    unsigned int requestsQueueSize{16};

    std::string sslCertFileName{"Servers/IntermediaryServer/cert.pem"};
    std::string sslKeyFileName{"Servers/IntermediaryServer/key.pem"};

    LoggerConfiguration loggerConfiguration{
        LoggerConfiguration::FileAlwaysOpenPolicy::ALWAYS_OPEN,
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

    bool operator==(const ServerConfiguration &otherConf) const {
      return port == otherConf.port &&
             requestHandlerAmount == otherConf.requestHandlerAmount &&
             applicationHandlerAmount == otherConf.applicationHandlerAmount &&
             responseHandlerAmount == otherConf.responseHandlerAmount &&
             requestsQueueSize == otherConf.requestsQueueSize &&
             sslCertFileName == otherConf.sslCertFileName &&
             sslKeyFileName == otherConf.sslKeyFileName &&
             loggerConfiguration == otherConf.loggerConfiguration;
    }
  };
}
#endif //HARBOR_CLS_SERVERCONFIGURATION_HPP