//
// Created by josephvalverde on 12/12/23.
//

#ifndef HTTPSERVER_PARSINGPOLICY_HPP
#define HTTPSERVER_PARSINGPOLICY_HPP

#include "json.hpp"
#include <string>
#include <list>
#include <functional>

#include "PolicyOperation.hpp"
#include "IPolicyOperation.hpp"

namespace HarborCLS {
  template<template<typename> class parsingPolicy, typename JsonFileType>
  concept IsParsingPolicy = requires {
    typename parsingPolicy<JsonFileType>;
  };

  template<typename dataType, typename JsonFileType>
  class ParsingPolicy {
    using Policy = IPolicyOperation<JsonFileType, dataType>;
    std::vector<std::unique_ptr<Policy>> policies;

  public:
    virtual void addAllPolicies() = 0;

    /**
     * @brief Deserialize json to data
     * @param json json to deserialize
     * @return deserialized data
     */
    void applyPoliciesOnDeserialization(dataType &data, JsonFileType &json) {
      for (auto &policy: this->policies) {
        policy->deserialize(data, json);
      }
    }

    /**
     * @brief Serialize data to json
     * @param data data to serialize
     * @param json json to serialize to
     */
    void applyPoliciesOnSerialization(dataType &data, JsonFileType &json) {
      for (auto &policy: this->policies) {
        policy->serialize(data, json);
      }
    }

  protected:
    /**
     * @brief Add policy to policies
     * @param key key to serialize to
     * @param value value to serialize
     */
    template<typename AttributeType>
    void addPolicy(const std::string &key, std::function<AttributeType &(dataType &)> value) {
      std::string noConstKey = key;
      auto newPolicy = std::make_unique<PolicyOperation<dataType, AttributeType>>(value, noConstKey);
      this->policies.push_back(std::move(newPolicy));
    }
  };
}

#endif //HTTPSERVER_PARSINGPOLICY_HPP