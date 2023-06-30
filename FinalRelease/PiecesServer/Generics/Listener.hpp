#ifndef LISTERNER_HPP
#define LISTERNER_HPP 

#include "Thread.hpp"
#include "Queue.hpp"

#include "Socket/Socket.hpp"

#include <iostream>
#include <string>

template <typename enqueutype>
class Listener : public virtual Thread {
 protected:
  Queue<enqueutype>* queue;

  Socket* listeningSocket;
  enqueutype stopCondition;
  enqueutype handlerStopCondition;

  bool stopThread;

  std::string listeningMessage;
  std::string stopMessage;
  std::string afterReceivedMessage;

  int stopPort;
  bool ssl;
  char socketType;

 public:
  Listener(Queue<enqueutype>* queue,
      Socket* listeningSocket,
      enqueutype stopCondition,
      enqueutype handlerStopCondition,
      int stopPort,
      bool ssl,
      char socketType,
      std::string& listeningMessage,
      std::string& afterReceivedMessage,
      std::string& stopMessage)
    : queue(queue)
    , listeningSocket(listeningSocket)
    , stopCondition(stopCondition)
    , handlerStopCondition(handlerStopCondition)
    , stopThread(false)
    , listeningMessage(listeningMessage)
    , stopMessage(stopMessage)
    , afterReceivedMessage(afterReceivedMessage)
    , stopPort(stopPort)
    , ssl(ssl)
    , socketType(socketType) {
  }

  ~Listener() {
  }

  void stop() {
    this->stopThread = true;

    this->unlockListen();

    this->queue->push(handlerStopCondition);
  }

 private:
  void listen() {
    while (true) {
      printf("%s", this->listeningMessage.data());

      enqueutype data = this->obtain();

      if (data == this->stopCondition || this->stopThread) {
        this->queue->push(data);
        printf("%s", this->stopMessage.data());
        //std::cout << this->stopMessage.data() << std::end;
        break;
      }

      printf("%s", this->afterReceivedMessage.data());
      //std::cout << this->afterReceivedMessage << std::endl;

      this->queue->push(data);
    }
  }

  void run() {
    this->listen();
  }

  virtual enqueutype obtain() = 0;

  // TODO: add UDP support
  void unlockListen() {
    Socket closingSocket(this->socketType, false);

    if (this->ssl) {
      closingSocket.SSLInit();
    }

    closingSocket.Connect("any IP", this->stopPort);
  }
};

#endif