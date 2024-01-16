//
// Created by josephvalverde on 12/15/23.
//

#ifndef HARBOR_CLS_IJSONHANDLER_HPP
#define HARBOR_CLS_IJSONHANDLER_HPP

namespace HarborCLS {

  template<typename dataType>
  class IJsonHandler {
  public:
    virtual ~IJsonHandler() = default;

    /**
     * @brief Deserialize jsonTo to dataType
     * @return deserialized data
     */
    virtual dataType deserialize() = 0;

    /**
     * @brief Serialize dataType to jsonTo
     * @return json string of serialized data
     */
    virtual std::string serialize() = 0;

    /**
     * @brief Get jsonTo value at key
     * @param key or name of field in jason
     * @return a std::nullopt if key does not exist or jsonfile not created successfully, otherwise the value at key
     */
    [[nodiscard]] virtual std::optional<std::string> jsonAt(const std::string &key) = 0;
  };
}

#endif //HARBOR_CLS_IJSONHANDLER_HPP
