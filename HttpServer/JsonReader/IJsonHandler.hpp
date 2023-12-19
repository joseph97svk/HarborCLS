//
// Created by josephvalverde on 12/15/23.
//

#ifndef LEGO_FIGURE_MAKER_IJSONHANDLER_HPP
#define LEGO_FIGURE_MAKER_IJSONHANDLER_HPP

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
};

#endif //LEGO_FIGURE_MAKER_IJSONHANDLER_HPP
