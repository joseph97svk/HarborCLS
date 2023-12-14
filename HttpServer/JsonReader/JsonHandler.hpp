//
// Created by josephvalverde on 12/12/23.
//

#ifndef HTTPSERVER_JSONHANDLER_HPP
#define HTTPSERVER_JSONHANDLER_HPP

#include "json.hpp"
#include "ParsingPolicy.hpp"
#include <variant>
#include <fstream>
#include <filesystem>
#include <optional>

template<typename dataType, typename ParsingPolicy>
class JsonHandler {
    std::optional<nlohmann::json> jsonToParse;
    std::optional<dataType> dataToSerialize;

    ParsingPolicy parsingPolicy;

public:
    explicit JsonHandler(const std::string& json, bool isPath)
        : parsingPolicy(),
          jsonToParse(),
          dataToSerialize() {
      this->parsingPolicy.addAllPolicies();

      if (!isPath) {
        this->jsonToParse = nlohmann::json::parse(json);
        return;
      }

      std::filesystem::path path(json);

      if (!(std::filesystem::exists(path) && path.has_filename())) {
        throw std::runtime_error("Invalid jsonTo path");
      }

      std::ifstream jsonFile(path);

      if (!jsonFile.is_open()) {
        throw std::runtime_error("Could not open jsonTo file");
      }

      nlohmann::json jsonBuffer;
      jsonFile >> jsonBuffer;

      this->jsonToParse = jsonBuffer;
    }

    explicit JsonHandler(dataType& data)
        : parsingPolicy(),
          jsonToParse(),
          dataToSerialize() {
      this->parsingPolicy.addAllPolicies();
      this->dataToSerialize = data;
    }

    dataType deserialize() {
      if (this->dataToSerialize.has_value()) {
        return this->dataToSerialize.value();
      }

      if (!this->jsonToParse.has_value()) {
        throw std::runtime_error("No jsonTo to deserialize");
      }

      dataType data;
      this->parsingPolicy.applyPoliciesOnDeserialization(data, this->jsonToParse.value());
      return data;
    }

    std::string serialize() {
      if (this->jsonToParse.has_value()) {
        return this->jsonToParse->dump();
      }

      nlohmann::json json;
      this->parsingPolicy.applyPoliciesOnSerialization(this->dataToSerialize, json);
      return json.dump();
    }
};

#endif //HTTPSERVER_JSONHANDLER_HPP