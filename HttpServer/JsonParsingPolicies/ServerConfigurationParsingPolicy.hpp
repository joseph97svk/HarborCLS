//
// Created by josephvalverde on 12/12/23.
//

#ifndef HTTPSERVER_SERVERCONFIGURATIONPARSINGPOLICY_HPP
#define HTTPSERVER_SERVERCONFIGURATIONPARSINGPOLICY_HPP

#include "../ServerConfiguration.hpp"
#include "../JsonReader/ParsingPolicy.hpp"

class ServerConfigurationParsingPolicy : public ParsingPolicy<ServerConfiguration> {
public:
  /**
   * @brief Adds all the policies to the parsing policy.
   */
  void addAllPolicies() override {
    this->addPolicy<unsigned int>("port",
                    [](ServerConfiguration& config) -> unsigned int& { return config.port; });
    this->addPolicy<unsigned int>("requestHandlerAmount",
                    [](ServerConfiguration& config) -> unsigned int& { return config.requestHandlerAmount; });
    this->addPolicy<unsigned int>("applicationHandlerAmount",
                    [](ServerConfiguration& config) -> unsigned int& { return config.applicationHandlerAmount; });
    this->addPolicy<unsigned int>("responseHandlerAmount",
                    [](ServerConfiguration& config) -> unsigned int& { return config.responseHandlerAmount; });
    this->addPolicy<std::string>("sslCertFileName",
                    [](ServerConfiguration& config) -> std::string& { return config.sslCertFileName; });
    this->addPolicy<std::string>("sslKeyFileName",
                    [](ServerConfiguration& config) -> std::string& { return config.sslKeyFileName; });
    this->addPolicy<unsigned int>("requestsQueueSize",
                    [](ServerConfiguration& config) -> unsigned int& { return config.requestsQueueSize; });
    this->addPolicy<std::string>("LoggerConfiguration:LoggerFilePath",
                    [](ServerConfiguration& config) -> std::string& { return config.loggerConfiguration.logFilePath; });
  }
};

#endif //HTTPSERVER_SERVERCONFIGURATIONPARSINGPOLICY_HPP
