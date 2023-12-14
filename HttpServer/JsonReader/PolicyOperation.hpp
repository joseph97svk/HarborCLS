//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_POLICYOPERATION_HPP
#define LEGO_FIGURE_MAKER_POLICYOPERATION_HPP

#include <functional>
#include <utility>

#include "json.hpp"

#include "IPolicyOperation.hpp"
#include "NlohmannJsonPolicy.hpp"

template<typename SourceType, typename AttributeType>
struct PolicyOperation : public NlohmannJsonPolicy<SourceType, AttributeType> {
    explicit PolicyOperation(std::function<AttributeType&(SourceType&)> operation, std::string& key)
        : operation(operation)
        , key(key){}

    void serialize(SourceType& data, nlohmann::json& json) override {
      AttributeType& attribute = this->operation(data);
      std::any anyAttribute = attribute;
      this->serializeOperation(this->key, json, anyAttribute);
    }

    void deserialize(SourceType& data, nlohmann::json& json) override {
      AttributeType& attribute = this->operation(data);

      std::any result = this->deserializeOperation(this->key, json);

      if (result.type() == typeid(AttributeType)) {
        AttributeType attributeBuffer = std::any_cast<AttributeType>(result);
        attribute = attributeBuffer;
      } else {
        throw std::runtime_error("Type mismatch");
      }
    }

private:
    std::function<AttributeType&(SourceType&)> operation;
    std::string key;
};

#endif //LEGO_FIGURE_MAKER_POLICYOPERATION_HPP
