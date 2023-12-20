#ifndef HANDLER_HPP
#define HANDLER_HPP

#include <iostream>

#include "Server/Concurrency/Thread.hpp"
#include "Server/Concurrency/Queue.hpp"
#include "MiddlewareMessage.hpp"

namespace HarborCLS {

  template<typename consumeDataType>
  class Handler : public virtual Thread {
  protected:
    MiddlewareBlockingQueue<consumeDataType>& _consumingQueue;


  public:
    explicit Handler(MiddlewareBlockingQueue<consumeDataType>& consumingQueue)
        : Thread()
        , _consumingQueue(consumingQueue) {
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
          [](Error<MessageErrors> &error) {
            std::string errorMessage = error;
            std::cerr << errorMessage << std::endl;
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