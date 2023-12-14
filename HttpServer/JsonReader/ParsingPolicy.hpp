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

template<typename dataType>
class ParsingPolicy {
    using Policy = IPolicyOperation<nlohmann::json, dataType>;
    std::vector<std::unique_ptr<Policy>> policies;

public:
    virtual void addAllPolicies() = 0;

    void applyPoliciesOnDeserialization(dataType& data, nlohmann::json& json) {
      for (auto& policy : this->policies) {
        policy->deserialize(data, json);
      }
    }

    void applyPoliciesOnSerialization(const dataType& data, nlohmann::json& json) {
      for (auto& policy : this->policies) {
        policy->serialize(data, json);
      }
    }

protected:
    template<typename AttributeType>
    void addPolicy(const std::string& key, std::function<AttributeType&(dataType&)> value) {
      std::string noConstKey = key;
      auto newPolicy = std::make_unique<PolicyOperation<dataType, AttributeType>>(value, noConstKey);
      this->policies.push_back(std::move(newPolicy));
    }
};

#endif //HTTPSERVER_PARSINGPOLICY_HPP