#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "Thread.hpp"
#include "Queue.hpp"
#include "Generics/common.hpp"

template <typename datatype>
class Handler : public virtual Thread {
 protected:
  Queue<datatype>* consumingQueue;

  datatype stopCondition;

 public:
  Handler(Queue<datatype>* consumingQueue,
      datatype stopCondition)
    : consumingQueue(consumingQueue)
    , stopCondition(stopCondition) {
  }

  ~Handler() {
  }

 protected:
  void handle() {
    while (true) {
      datatype data = this->consumingQueue->pop();

      if (data == this->stopCondition) {
        break;
      }

      handleSingle(data);
    }
  }

  void run() {
    this->handle();
  }

  virtual void handleSingle(datatype handlingData);
};

#endif