//
// Created by josephvalverde on 1/11/24.
//

#ifndef HARBOR_CLS_ICONFIGURATION_HPP
#define HARBOR_CLS_ICONFIGURATION_HPP

#include <string>
#include <optional>

namespace HarborCLS {

  template<typename ConfigurationClassType>
  class IConfiguration {
  public:
    virtual ~IConfiguration() = default;

    virtual std::optional<std::string> get(const std::string &key) = 0;

    virtual ConfigurationClassType& getConfiguration() = 0;
  };

}

#endif //HARBOR_CLS_ICONFIGURATION_HPP
