//
// Created by josephvalverde on 12/12/23.
//

#include "Concurrency/Thread.hpp"
#include "Concurrency/Queue.hpp"
#include "../Middleware/ListenerMessageBundle.hpp"
#include <memory>

template <typename enqueueType, typename SocketType>
class Listener : public virtual Thread {
protected:
    Queue<enqueueType>* queue;
    std::shared_ptr<SocketType> listeningSocket;
    ListenerMessageBundle messageBundle;

    enqueueType stopCondition;
    enqueueType handlerStopCondition;

    unsigned int stopPort;

    bool udp = false;
    bool stopThread = false;

public:
    Listener(Queue<enqueueType>* queue,
             std::shared_ptr<SocketType> listeningSocket,
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
      SocketType closingSocket(this->listeningSocket->getType(), false);
      closingSocket.connect("any IP", this->stopPort);
    }
};