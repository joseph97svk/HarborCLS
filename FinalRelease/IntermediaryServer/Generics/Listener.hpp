#ifndef LISTERNER_HPP
#define LISTERNER_HPP 

#include "Thread.hpp"
#include "Queue.hpp"

#include "Socket/Socket.hpp"

#include <iostream>
#include <string>

class Listener : public virtual Thread {
 protected:
  Queue<Socket*>* queue;

  Socket* listeningSocket;
  Socket* stopCondition;
  Socket* handlerStopCondition;

  bool stopThread;

  std::string stopMessage;
  std::string afterReceivedMessage;

  int stopPort;
  bool ssl;
  char socketType;

 public:
  explicit Listener(Queue<Socket*>* queue,
      Socket* listeningSocket,
      Socket* stopCondition,
      Socket* handlerStopCondition,
      int stopPort,
      bool ssl,
      char socketType,
      std::string& afterReceivedMessage,
      std::string& stopMessage)
    : Thread()
    , queue(queue)
    , listeningSocket(listeningSocket)
    , stopCondition(stopCondition)
    , handlerStopCondition(handlerStopCondition)
    , stopThread(false)
    , stopMessage(stopMessage)
    , afterReceivedMessage(afterReceivedMessage)
    , stopPort(stopPort)
    , ssl(ssl)
    , socketType(socketType) {
  }

  virtual ~Listener() {
  }

  void stop() {
    this->stopThread = true;

    this->unlockListen();
  }

 private:
  void listen() {
    while (true) {
      Socket* data = this->obtain();

      if (data == this->stopCondition || this->stopThread) {
        this->queue->push(data);
        //std::cout << this->stopMessage << std::end;
        break;
      }

      //std::cout << this->afterReceivedMessage << std::endl;

      this->queue->push(data);
    }
  }

  virtual void run() override {
    this->listen();
  }

  virtual Socket* obtain() = 0;

  void unlockListen() {
    Socket closingSocket(this->socketType, false);

    if (this->ssl) {
      closingSocket.SSLInit();
    }

    closingSocket.Connect("any IP", this->stopPort);
  }
};

#endif