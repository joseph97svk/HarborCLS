#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "Concurrency/Thread.hpp"
#include "Concurrency/Queue.hpp"
#include "common.hpp"

template <typename consumeDataType>
class Handler : public virtual Thread {
 protected:
  Queue<consumeDataType>* consumingQueue;

  consumeDataType stopCondition;

 public:
  explicit Handler(Queue<consumeDataType>* consumingQueue,
                   consumeDataType stopCondition)
    : Thread()
    , consumingQueue(consumingQueue)
    , stopCondition(stopCondition) {
  }

  ~Handler() override = default;

 protected:
  void handle() {
    while (true) {
      consumeDataType data = this->consumingQueue->pop();

      if (data == this->stopCondition) {
        std::cout << "Handler stopping" << std::endl;
        optionalToEnd();
        break;
      }

      handleSingle(data);
    }
  }

  void run() override {
    this->handle();
  }

  virtual void optionalToEnd () = 0;


  virtual void handleSingle(consumeDataType handlingData) = 0;
};

#endif