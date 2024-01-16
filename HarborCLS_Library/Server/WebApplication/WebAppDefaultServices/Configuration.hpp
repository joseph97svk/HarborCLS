//
// Created by josephvalverde on 1/16/24.
//

#ifndef HARBOR_CLS_CONFIGURATION_HPP
#define HARBOR_CLS_CONFIGURATION_HPP

#include "IConfiguration.hpp"

#include "../../ServerConfiguration.hpp"

namespace HarborCLS {

  class Configuration : public IConfiguration<ServerConfiguration> {
    ServerConfiguration _configuration;
    std::optional<JsonHandler<ServerConfiguration, ServerConfigurationParsingPolicy>> _jsonHandler;

  public:
    explicit Configuration(std::string_view configurationFilePath)
    : _jsonHandler(
        std::make_optional<JsonHandler<ServerConfiguration, ServerConfigurationParsingPolicy>>(
            std::string(configurationFilePath), true)) {
      _configuration = _jsonHandler->deserialize();
    }

    explicit Configuration(ServerConfiguration configuration)
        : _configuration(configuration) {
    }

    std::optional<std::string> get(const std::string &key) override {
      if (!_jsonHandler) {
        return std::nullopt;
      }

      auto result = _jsonHandler->jsonAt(key);

      if (!result) {
        return std::nullopt;
      }

      if (result.value()[0] == '"') {
        result = result->substr(1, result->size() - 2);
      }

      if (result.value()[result.value().length()]) {
        result = result->substr(0, result->size() - 1);
      }

      return result;
    }

    ServerConfiguration& getConfiguration() override {
      return _configuration;
    }
  };

}

#endif //HARBOR_CLS_CONFIGURATION_HPP
