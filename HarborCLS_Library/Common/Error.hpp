//
// Created by josephvalverde on 12/20/23.
//

#ifndef HARBOR_CLS_ERROR_H
#define HARBOR_CLS_ERROR_H

#include <string>
#include <source_location>

namespace HarborCLS {
  /**
   * @brief ErrorTypeEnum must be an enum class
   * @tparam T type to check
   */
  template<typename T>
  concept ErrorTypeEnum =
      std::is_enum_v<T>
      && !std::is_convertible_v<T, std::underlying_type_t<T>>;

  /**
   * @brief Error class
   * @tparam ErrorType should be an enum class
   */
  template<ErrorTypeEnum ErrorType>
  class Error {
    std::string _message{};
    std::source_location _location{};
    ErrorType _errorType;

  public:
    Error(std::string message, std::source_location location, ErrorType errorType)
        : _message(std::move(message))
        , _location(location)
        , _errorType(errorType) {
    }

    std::string& what() {
      return _message;
    }

    operator const std::string() const {
      return _message
      + " in " + _location.file_name()
      + ": " + std::to_string(_location.line())
      + ": " + std::to_string(_location.column());
    }

    ErrorType errorType() const noexcept {
      return _errorType;
    }
  };
}

#endif //HARBOR_CLS_ERROR_H
