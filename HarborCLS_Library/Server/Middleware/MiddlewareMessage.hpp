//
// Created by josephvalverde on 12/20/23.
//

#ifndef HARBOR_CLS_MIDDLEWAREMESSAGE_H
#define HARBOR_CLS_MIDDLEWAREMESSAGE_H

#include <optional>
#include <expected>
#include <variant>

#include "Common/Error.hpp"
#include "../Concurrency/Queue.hpp"

template<class... Ts>
struct overloaded : Ts ... {
  using Ts::operator()...;
};

namespace HarborCLS {
  class StopCondition {
  };

  enum class MessageErrors {
    OK = 0,
    GENERIC_ERROR
  };

  template<typename MessageDataType>
  class MiddlewareMessage {
  public:
    explicit MiddlewareMessage(MessageDataType messageDataType)
        : _content(std::move(messageDataType)) {}

    explicit MiddlewareMessage(StopCondition stopCondition)
        : _content(stopCondition) {}

    explicit MiddlewareMessage(Error<MessageErrors> error)
        : _content(std::move(error)) {}

    std::expected<MessageDataType, std::variant<StopCondition, Error<MessageErrors>>> getContent() {
      return std::visit(overloaded{
        [](MessageDataType &messageDataType) ->
            std::expected<MessageDataType, std::variant<StopCondition, Error<MessageErrors>>> {
          return messageDataType;
        },
        [](StopCondition &stopCondition) ->
            std::expected<MessageDataType, std::variant<StopCondition, Error<MessageErrors>>>{
          return std::unexpected(stopCondition);
        },
        [](Error<MessageErrors> &error) ->
            std::expected<MessageDataType, std::variant<StopCondition, Error<MessageErrors>>>{
          return std::unexpected(error);
        }
      }, _content);
    }

  protected:
    std::variant<MessageDataType, StopCondition, Error<MessageErrors>> _content;
  };

  template <typename ContentType>
  using MiddlewareBlockingQueue = BlockingQueue<MiddlewareMessage<ContentType>>;
}

#endif //HARBOR_CLS_MIDDLEWAREMESSAGE_H
