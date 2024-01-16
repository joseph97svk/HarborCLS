//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_ICONFIGURATION_HPP
#define HARBOR_CLS_ICONFIGURATION_HPP

#include <string>
#include <optional>

class IConfiguration {
public:
    virtual ~IConfiguration() = default;

    std::optional<std::string> get(const std::string& key);
};

#endif //HARBOR_CLS_ICONFIGURATION_HPP
