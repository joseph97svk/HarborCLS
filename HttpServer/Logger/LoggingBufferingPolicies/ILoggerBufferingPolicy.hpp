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

  /**
   * @brief Buffer a message.
   * @param message The message to buffer.
   * @return The buffered message if the buffer is full, otherwise an empty optional.
   */
  virtual std::optional<std::string> buffer(std::string& message) = 0;
};

#endif //LEGO_FIGURE_MAKER_ILOGGERBUFFERINGPOLICY_HPP
