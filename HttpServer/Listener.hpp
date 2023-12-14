#ifndef LISTERNER_HPP
#define LISTERNER_HPP 

#include "Concurrency/Thread.hpp"
#include "Concurrency/Queue.hpp"
#include "Middleware/ListenerMessageBundle.hpp"

#include "Socket/Socket.hpp"

#include <iostream>
#include <string>
#include <utility>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

template <typename enqueueType>
class Listener : public virtual Thread {
 protected:
  Queue<enqueueType>* queue;
  std::shared_ptr<Socket> listeningSocket;
  ListenerMessageBundle messageBundle;

  enqueueType stopCondition;
  enqueueType handlerStopCondition;

  int stopPort;

  bool udp = false;
  bool stopThread = false;

 public:
  Listener(Queue<enqueueType>* queue,
           std::shared_ptr<Socket> listeningSocket,
           ListenerMessageBundle& messageBundle,
           enqueueType stopCondition,
           enqueueType handlerStopCondition,
           unsigned int stopPort
           )
    : queue(queue)
    , listeningSocket(std::move(listeningSocket))
    , messageBundle(std::move(messageBundle))
    , stopCondition(stopCondition)
    , handlerStopCondition(handlerStopCondition)
    , stopPort(stopPort) {
  }

  ~Listener() override = default;

  void stop() {
    this->stopThread = true;

    this->unlockListen();

    this->queue->push(handlerStopCondition);
  }

  void setAsUDP() {
    this->udp = true;
  }

 private:
  virtual void listen() {
    while (true) {
      printf("%s", this->messageBundle.listeningMessage.c_str());

      enqueueType data = this->obtain();

      if (data == this->stopCondition || this->stopThread) {
        this->queue->push(data);
        printf("%s", this->messageBundle.stopMessage.c_str());

        break;
      }

      printf("%s", this->messageBundle.afterReceivedMessage.c_str());

      this->queue->push(data);
    }
  }

  void run() override {
    this->listen();
  }

  virtual enqueueType obtain() = 0;

  void unlockListen() {
    Socket closingSocket(this->listeningSocket->getType(), false);

    if (this->udp) {
      sockaddr_in sockinfo{};
      memset(&sockinfo, 0, sizeof(sockaddr_in));

      sockinfo.sin_family = AF_INET;
      sockinfo.sin_port = htons(this->stopPort);
      sockinfo.sin_addr.s_addr = INADDR_ANY;

      closingSocket.sendTo((void*) "sup", 3, &sockinfo);

      return;
    }

    if (this->listeningSocket->isSSL()) {
      closingSocket.SSLInit();
      closingSocket.SSLConnect((char*) "any IP", this->stopPort);

      return;
    }

    closingSocket.Connect("any IP", this->stopPort);
  }
};

#endif