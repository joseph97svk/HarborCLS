#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>
#include <utility>

#include "Server/Concurrency/Thread.hpp"
#include "Server/Concurrency/Queue.hpp"
#include "MiddlewareMessage.hpp"
#include "../../Logger/ILogger.hpp"

namespace HarborCLS {

  template<typename consumeDataType>
  class Handler : public virtual Thread {
  protected:
    MiddlewareBlockingQueue<consumeDataType>& _consumingQueue;

    std::shared_ptr<ILogger> _logger;

  public:
    explicit Handler(MiddlewareBlockingQueue<consumeDataType>& consumingQueue
                     , std::shared_ptr<ILogger> logger)
        : Thread()
        , _consumingQueue(consumingQueue)
        , _logger(std::move(logger)) {
    }

    ~Handler() override = default;

  protected:
    void handle() {
      while (true) {
        MiddlewareMessage<consumeDataType> data = _consumingQueue.pop();

        std::expected<consumeDataType, std::variant<StopCondition, Error<MessageErrors>>> content = data.getContent();

        if (content) {
          handleSingle(content.value());
          continue;
        }

        std::visit(overloaded{
          [this](StopCondition &stopCondition) {
            this->optionalToEnd();
          },
          [this](Error<MessageErrors> &error) {
            std::string errorMessage = error;
            _logger->error("Error while obtaining data: " + errorMessage);
          }
        }, content.error());

        break;
      }
    }

    void run() override {
      this->handle();
    }

    virtual void optionalToEnd() = 0;

    virtual void handleSingle(consumeDataType handlingData) = 0;
  };
}
#endif