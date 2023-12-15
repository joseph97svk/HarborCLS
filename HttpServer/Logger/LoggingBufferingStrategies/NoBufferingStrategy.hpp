//
// Created by josephvalverde on 12/13/23.
//

#ifndef LEGO_FIGURE_MAKER_NOBUFFERINGSTRATEGY_HPP
#define LEGO_FIGURE_MAKER_NOBUFFERINGSTRATEGY_HPP

#include "ILoggerBufferingStrategy.hpp"

class NoBufferingStrategy : public ILoggerBufferingStrategy {
public:
  NoBufferingStrategy() = default;

  ~NoBufferingStrategy() override = default;

  std::optional<std::string> buffer(std::string& message) override {
    return message;
  }
};

#endif //LEGO_FIGURE_MAKER_NOBUFFERINGSTRATEGY_HPP
