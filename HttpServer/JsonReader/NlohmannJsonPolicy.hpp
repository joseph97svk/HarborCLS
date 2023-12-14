//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_NLOHMANNJSONPOLICY_HPP
#define LEGO_FIGURE_MAKER_NLOHMANNJSONPOLICY_HPP

#include <functional>

#include "PolicyOperation.hpp"

template<typename SourceType, typename AttributeType>
struct NlohmannJsonPolicy : IPolicyOperation<nlohmann::json, SourceType>{
    ~NlohmannJsonPolicy() override = default;

    void serialize(SourceType& data, nlohmann::json& json) override = 0;
    void deserialize(SourceType& data, nlohmann::json& json) override = 0;

protected:
    void serializeOperation(std::string& key, nlohmann::json& json, std::any& dataInput) override {
      json[key] = std::any_cast<AttributeType>(dataInput);
    }

    std::any deserializeOperation(std::string& key, nlohmann::json& json) override {
      AttributeType attribute;

      json.at(key).get_to(attribute);

      return attribute;
    }
};

#endif //LEGO_FIGURE_MAKER_NLOHMANNJSONPOLICY_HPP
