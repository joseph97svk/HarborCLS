#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "Thread.hpp"
#include "Queue.hpp"

template <typename datatype>
class Handler : public virtual Thread {
 protected:
  Queue<datatype>* consumingQueue;

  datatype stopCondition;

 public:
  explicit Handler(Queue<datatype>* consumingQueue,
      datatype stopCondition)
    : Thread()
    , consumingQueue(consumingQueue)
    , stopCondition(stopCondition) {
  }

  virtual ~Handler() {
  }

 protected:
  void handle() {
    while (true) {
      datatype data = this->consumingQueue->pop();

      if (data == this->stopCondition) {
        std::cout << "Handler stopping" << std::endl;
        break;
      }

      handleSingle(data);
    }
  }

  virtual void run() override {
    this->handle();
  }

  virtual void handleSingle(datatype handlingData) = 0;
};

#endif