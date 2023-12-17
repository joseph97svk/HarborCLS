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
  Queue<enqueueType>* _queue;
  std::shared_ptr<Socket> _listeningSocket;
  ListenerMessageBundle _messageBundle;

  enqueueType _stopCondition;
  enqueueType _handlerStopCondition;

  int _stopPort;

  bool _udp = false;
  bool _stopThread = false;

 public:
  Listener(Queue<enqueueType>* queue,
           std::shared_ptr<Socket> listeningSocket,
           ListenerMessageBundle& messageBundle,
           enqueueType stopCondition,
           enqueueType handlerStopCondition,
           unsigned int stopPort
           )
    : _queue(queue)
    , _listeningSocket(std::move(listeningSocket))
    , _messageBundle(std::move(messageBundle))
    , _stopCondition(stopCondition)
    , _handlerStopCondition(handlerStopCondition)
    , _stopPort(stopPort) {
  }

  ~Listener() override = default;

  void stop() {
    this->_stopThread = true;

    this->unlockListen();

    this->_queue->push(_handlerStopCondition);
  }

  void setAsUDP() {
    this->_udp = true;
  }

 private:
  virtual void listen() {
    while (true) {
      printf("%s", this->_messageBundle.listeningMessage.c_str());

      enqueueType data = this->obtain();

      if (data == this->_stopCondition || this->_stopThread) {
        this->_queue->push(data);
        printf("%s", this->_messageBundle.stopMessage.c_str());

        break;
      }

      printf("%s", this->_messageBundle.afterReceivedMessage.c_str());

      this->_queue->push(data);
    }
  }

  void run() override {
    this->listen();
  }

  virtual enqueueType obtain() = 0;

  void unlockListen() {
    Socket closingSocket(this->_listeningSocket->getType(), false);

    if (this->_udp) {
      sockaddr_in sockinfo{};
      memset(&sockinfo, 0, sizeof(sockaddr_in));

      sockinfo.sin_family = AF_INET;
      sockinfo.sin_port = htons(this->_stopPort);
      sockinfo.sin_addr.s_addr = INADDR_ANY;

      closingSocket.sendTo((void*) "sup", 3, &sockinfo);

      return;
    }

    if (this->_listeningSocket->isSSL()) {
      closingSocket.SSLInit();
      closingSocket.SSLConnect((char*) "any IP", this->_stopPort);

      return;
    }

    closingSocket.Connect("any IP", this->_stopPort);
  }
};

#endif