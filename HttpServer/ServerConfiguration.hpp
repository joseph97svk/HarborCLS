//
// Created by josephvalverde on 12/11/23.
//

#include <string>

struct ServerConfiguration {
    unsigned int port{};

    unsigned int requestHandlerAmount{};
    unsigned int applicationHandlerAmount{};
    unsigned int responseHandlerAmount{};
    unsigned int requestsQueueSize{};

    std::string sslCertFileName{};
    std::string sslKeyFileName{};

    ServerConfiguration() = default;

    ServerConfiguration(unsigned int port, unsigned int requestHandlerAmount, unsigned int applicationHandlerAmount,
                        unsigned int responseHandlerAmount, unsigned int requestsQueueSize,
                        std::string sslCertFileName, std::string sslKeyFileName) :
            port(port), requestHandlerAmount(requestHandlerAmount), applicationHandlerAmount(applicationHandlerAmount),
            responseHandlerAmount(responseHandlerAmount), requestsQueueSize(requestsQueueSize),
            sslCertFileName(std::move(sslCertFileName)), sslKeyFileName(std::move(sslKeyFileName)) {}

    ServerConfiguration(ServerConfiguration&&) = default;
    ServerConfiguration(const ServerConfiguration&) = default;
    ServerConfiguration& operator=(ServerConfiguration&&) = default;
    ServerConfiguration& operator=(const ServerConfiguration&) = default;
    ~ServerConfiguration() = default;

    [[nodiscard]] static ServerConfiguration createDefaultConfiguration() {
        return {8080, 1, 1, 1, 16, "", ""};
    }

    bool isSslEnabled () {
        return !sslCertFileName.empty() && !sslKeyFileName.empty();
    }
};