//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_IPOLICYOPERATION_HPP
#define HARBOR_CLS_IPOLICYOPERATION_HPP

#include <any>
#include <string>

namespace HarborCLS {

  template<typename JsonType, typename SourceType>
  struct IPolicyOperation {
  public:
    virtual ~IPolicyOperation() = default;

    /**
     * @brief Serialize data to json
     * @param data data to serialize
     * @param json json to serialize to
     */
    virtual void serialize(SourceType &data, JsonType &json) = 0;

    /**
     * @brief Deserialize json to data
     * @param data data to deserialize to
     * @param json json to deserialize
     */
    virtual void deserialize(SourceType &data, JsonType &jsonType) = 0;

  protected:
    /**
     * @brief Serialize dataInput to json
     * @param key key to serialize to
     * @param json json to serialize to
     * @param dataInput data to serialize
     */
    virtual void serializeOperation(std::string &key, JsonType &json, std::any &dataInput) = 0;

    /**
     * @brief Deserialize json to dataInput
     * @param key key to deserialize
     * @param json json to deserialize
     * @return deserialized data
     */
    virtual std::any deserializeOperation(std::string &key, JsonType &json) = 0;
  };
}

#endif //HARBOR_CLS_IPOLICYOPERATION_HPP
