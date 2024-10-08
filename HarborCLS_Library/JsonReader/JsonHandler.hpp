//
// Created by josephvalverde on 12/12/23.
//

#ifndef HTTPSERVER_JSONHANDLER_HPP
#define HTTPSERVER_JSONHANDLER_HPP

#include <variant>
#include <fstream>
#include <filesystem>
#include <optional>

#include "IJsonHandler.hpp"

#include "json.hpp"
#include "ParsingPolicy.hpp"

namespace HarborCLS {

  template<typename dataType, template<typename> typename ParsingPolicy, typename JsonFileType = nlohmann::json>
      requires IsParsingPolicy<ParsingPolicy, JsonFileType>
  class JsonHandler : public IJsonHandler<dataType> {
    std::optional<JsonFileType> _jsonToParse {};
    std::optional<dataType> _dataToSerialize {};

    ParsingPolicy<JsonFileType> _parsingPolicy {};

  public:
    /**
     * @brief constructor for jsonTo handler
     * @param json string or path to jsonTo file
     * @param isPath whether provided string is a path or not
     */
    explicit JsonHandler(const std::string &json, bool isPath) {
      _parsingPolicy.addAllPolicies();

      if (!isPath) {
        _jsonToParse = JsonFileType::parse(json);
        return;
      }

      std::filesystem::path path(json);

      if (!std::filesystem::exists(path)) {
        throw std::runtime_error("Invalid jsonTo path");
      }

      std::ifstream jsonFile(path);

      if (!jsonFile.is_open()) {
        throw std::runtime_error("Could not open jsonTo file");
      }

      JsonFileType jsonBuffer;
      jsonFile >> jsonBuffer;

      _jsonToParse = jsonBuffer;
    }

    /**
     * @brief constructor for jsonTo handler
     * @param data to serialize
     */
    explicit JsonHandler(dataType &data)
        : _parsingPolicy(),
          _jsonToParse(),
          _dataToSerialize() {
      _parsingPolicy.addAllPolicies();
      _dataToSerialize = data;
    }

    /**
     * @brief constructor for jsonTo handler
     * @param data to serialize
     * @param parsingPolicy to apply
     */
    [[nodiscard]] dataType deserialize() override {
      if (_dataToSerialize.has_value()) {
        return _dataToSerialize.value();
      }

      if (!_jsonToParse.has_value()) {
        throw std::runtime_error("No jsonTo to deserialize");
      }

      dataType data;
      _parsingPolicy.applyPoliciesOnDeserialization(data, this->_jsonToParse.value());
      return data;
    }

    /**
     * @brief constructor for jsonTo handler
     * @param data to serialize
     * @param parsingPolicy to apply
     */
    [[nodiscard]] std::string serialize() override {
      if (_jsonToParse.has_value()) {
        return _jsonToParse->dump();
      }

      JsonFileType json;
      _parsingPolicy.applyPoliciesOnSerialization(*_dataToSerialize, json);
      return json.dump();
    }

    /**
     * @brief Get jsonTo value at key
     * @param key or name of field in jason
     * @return a std::nullopt if key does not exist or jsonfile not created successfully, otherwise the value at key
     */
    [[nodiscard]] std::optional<std::string> jsonAt(const std::string &key) override {
      if (!_jsonToParse.has_value()) {
        return std::nullopt;
      }

      if (!_jsonToParse->contains(key)) {
        return std::nullopt;
      }

      return _jsonToParse->at(key).dump();
    }
  };
}

#endif //HTTPSERVER_JSONHANDLER_HPP