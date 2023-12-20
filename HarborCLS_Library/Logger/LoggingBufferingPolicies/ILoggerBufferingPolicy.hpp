//
// Created by josephvalverde on 12/13/23.
//

#ifndef HARBOR_CLS_ILOGGERBUFFERINGPOLICY_HPP
#define HARBOR_CLS_ILOGGERBUFFERINGPOLICY_HPP

#include <optional>
#include <string>

namespace HarborCLS {

  class ILoggerBufferingPolicy {
  public:
    virtual ~ILoggerBufferingPolicy() = default;

    /**
     * @brief Buffer a message.
     * @param message The message to buffer.
     * @return The buffered message if the buffer is full, otherwise an empty optional.
     */
    virtual std::optional<std::string> buffer(std::string &message) = 0;
  };
}

#endif //HARBOR_CLS_ILOGGERBUFFERINGPOLICY_HPP
