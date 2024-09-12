//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_NLOHMANNJSONPOLICY_HPP
#define HARBOR_CLS_NLOHMANNJSONPOLICY_HPP

#include <functional>

#include "PolicyOperation.hpp"

namespace HarborCLS {

  template<typename SourceType, typename AttributeType>
  struct NlohmannJsonPolicy : IPolicyOperation<nlohmann::json, SourceType> {
    ~NlohmannJsonPolicy() override = default;

    /**
     * @brief Serialize data to json
     * @param data data to serialize
     * @param json json to serialize to
     */
    void serialize(SourceType &data, nlohmann::json &json) override = 0;

    /**
     * @brief Deserialize json to data
     * @param data data to deserialize to
     * @param json json to deserialize
     */
    void deserialize(SourceType &data, nlohmann::json &json) override = 0;

  protected:
    /**
     * @brief Serialize dataInput to json
     * @param key key to serialize to
     * @param json json to serialize to
     * @param dataInput data to serialize
     */
    void serializeOperation(std::string &key, nlohmann::json &json, std::any &dataInput) override {
      json[key] = std::any_cast<AttributeType>(dataInput);
    }

    /**
     * @brief Deserialize json to dataInput
     * @param key key to deserialize
     * @param json json to deserialize
     * @return deserialized data
     */
    std::any deserializeOperation(std::string &key, nlohmann::json &json) override {
      AttributeType attribute;

      json.at(key).get_to(attribute);

      return attribute;
    }
  };
}

#endif //HARBOR_CLS_NLOHMANNJSONPOLICY_HPP
