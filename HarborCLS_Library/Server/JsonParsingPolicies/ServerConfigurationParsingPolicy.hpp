//
// Created by josephvalverde on 12/12/23.
//

#ifndef HTTPSERVER_SERVERCONFIGURATIONPARSINGPOLICY_HPP
#define HTTPSERVER_SERVERCONFIGURATIONPARSINGPOLICY_HPP

#include "../ServerConfiguration.hpp"
#include "../../JsonReader/ParsingPolicy.hpp"

namespace HarborCLS {
  template<typename jsonDataType>
  class ServerConfigurationParsingPolicy : public ParsingPolicy<ServerConfiguration, jsonDataType> {
  public:
    /**
     * @brief Adds all the policies to the parsing policy.
     */
    void addAllPolicies() override {
      this->template addPolicy<unsigned int>("port",
                                    [](ServerConfiguration &config) -> unsigned int & { return config.port; });
      this->template addPolicy<unsigned int>("requestHandlerAmount",
                                    [](ServerConfiguration &config) -> unsigned int & { return config.requestHandlerAmount; });
      this->template addPolicy<unsigned int>("applicationHandlerAmount",
                                    [](ServerConfiguration &config) -> unsigned int & { return config.applicationHandlerAmount; });
      this->template addPolicy<unsigned int>("responseHandlerAmount",
                                    [](ServerConfiguration &config) -> unsigned int & { return config.responseHandlerAmount; });
      this->template addPolicy<std::string>("sslCertFileName",
                                   [](ServerConfiguration &config) -> std::string & { return config.sslCertFileName; });
      this->template addPolicy<std::string>("sslKeyFileName",
                                   [](ServerConfiguration &config) -> std::string & { return config.sslKeyFileName; });
      this->template addPolicy<unsigned int>("requestsQueueSize",
                                    [](ServerConfiguration &config) -> unsigned int & { return config.requestsQueueSize; });
      this->template addPolicy<std::string>("LoggerConfiguration:LoggerFilePath",
                                   [](ServerConfiguration &config) -> std::string & { return config.loggerConfiguration.logFilePath; });
    }
  };
}

#endif //HTTPSERVER_SERVERCONFIGURATIONPARSINGPOLICY_HPP
