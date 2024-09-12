//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_POLICYOPERATION_HPP
#define HARBOR_CLS_POLICYOPERATION_HPP

#include <functional>
#include <utility>

#include "json.hpp"

#include "IPolicyOperation.hpp"
#include "NlohmannJsonPolicy.hpp"

namespace HarborCLS {

  template<typename SourceType, typename AttributeType>
  struct PolicyOperation : public NlohmannJsonPolicy<SourceType, AttributeType> {
    /**
     * @brief Serialize data to json
     * @param data data to serialize
     * @param json json to serialize to
     */
    explicit PolicyOperation(std::function<AttributeType &(SourceType &)> operation, std::string &key)
        : _operation(operation), _key(key) {}

    /**
     * @brief Deserialize json to data
     * @param data data to deserialize to
     * @param json json to deserialize
     */
    void serialize(SourceType &data, nlohmann::json &json) override {
      AttributeType &attribute = this->_operation(data);
      std::any anyAttribute = attribute;
      this->serializeOperation(this->_key, json, anyAttribute);
    }

    /**
     * @brief Deserialize json to data
     * @param data data to deserialize to
     * @param json json to deserialize
     */
    void deserialize(SourceType &data, nlohmann::json &json) override {
      AttributeType &attribute = this->_operation(data);

      std::any result = this->deserializeOperation(this->_key, json);

      if (result.type() == typeid(AttributeType)) {
        AttributeType attributeBuffer = std::any_cast<AttributeType>(result);
        attribute = attributeBuffer;
      } else {
        throw std::runtime_error("Type mismatch");
      }
    }

  private:
    std::function<AttributeType &(SourceType &)> _operation;
    std::string _key;
  };
}

#endif //HARBOR_CLS_POLICYOPERATION_HPP
