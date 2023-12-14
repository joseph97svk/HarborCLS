//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_IPOLICYOPERATION_HPP
#define LEGO_FIGURE_MAKER_IPOLICYOPERATION_HPP

#include <any>
#include <string>

template <typename JsonType, typename SourceType>
struct IPolicyOperation {
public:
    virtual ~IPolicyOperation() = default;
    virtual void serialize(SourceType& data, JsonType& json) = 0;
    virtual void deserialize(SourceType& data, JsonType& jsonType) = 0;

protected:
    virtual void serializeOperation(std::string& key, JsonType& json, std::any& dataInput) = 0;
    virtual std::any deserializeOperation(std::string& key, JsonType& json) = 0;
};

#endif //LEGO_FIGURE_MAKER_IPOLICYOPERATION_HPP
