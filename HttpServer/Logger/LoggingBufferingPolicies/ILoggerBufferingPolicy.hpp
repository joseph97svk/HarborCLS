//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_ILOGGERBUFFERINGPOLICY_HPP
#define LEGO_FIGURE_MAKER_ILOGGERBUFFERINGPOLICY_HPP

#include <optional>
#include <string>

class ILoggerBufferingPolicy {
public:
  virtual ~ILoggerBufferingPolicy() = default;

  virtual std::optional<std::string> buffer(std::string& message) = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGERBUFFERINGPOLICY_HPP
