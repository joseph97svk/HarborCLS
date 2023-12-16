//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_NOBUFFERINGPOLICY_HPP
#define LEGO_FIGURE_MAKER_NOBUFFERINGPOLICY_HPP

#include "ILoggerBufferingPolicy.hpp"

class NoBufferingPolicy : public ILoggerBufferingPolicy {
public:
  NoBufferingPolicy() = default;

  ~NoBufferingPolicy() override = default;

  std::optional<std::string> buffer(std::string& message) override {
    return message;
  }
};

#endif //LEGO_FIGURE_MAKER_NOBUFFERINGPOLICY_HPP
