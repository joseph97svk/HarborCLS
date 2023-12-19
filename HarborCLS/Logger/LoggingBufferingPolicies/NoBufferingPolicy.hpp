//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_NOBUFFERINGPOLICY_HPP
#define HARBOR_CLS_NOBUFFERINGPOLICY_HPP

#include "ILoggerBufferingPolicy.hpp"

namespace HarborCLS {

  class NoBufferingPolicy : public ILoggerBufferingPolicy {
  public:
    NoBufferingPolicy() = default;

    ~NoBufferingPolicy() override = default;

    /**
     * @brief Buffer a message.
     * @param message The message to buffer.
     * @return The buffered message if the buffer is full, otherwise an empty optional.
     */
    std::optional<std::string> buffer(std::string &message) override {
      return message;
    }
  };
}

#endif //HARBOR_CLS_NOBUFFERINGPOLICY_HPP
