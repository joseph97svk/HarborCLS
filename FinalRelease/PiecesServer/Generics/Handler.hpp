#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "Thread.hpp"
#include "Queue.hpp"
#include "Socket/Socket.hpp"

class Handler : public virtual Thread {
 protected:
  Queue<Socket*>* consumingQueue;

  Socket* stopCondition;

 public:
  Handler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition)
    : consumingQueue(consumingQueue)
    , stopCondition(stopCondition) {
  }

  ~Handler() {
  }

 protected:
  void handle() {
    while (true) {
      Socket* data = this->consumingQueue->pop();

      if (data == this->stopCondition) {
        break;
      }

      handleSingle(data);
    }
  }

  void run() {
    this->handle();
  }

  virtual void handleSingle(Socket* handlingSocket);
};

#endif