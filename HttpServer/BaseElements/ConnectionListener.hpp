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
  Queue<enqueueType>* _queue {};
  std::shared_ptr<SocketType> _listeningSocket {};
  ListenerMessageBundle _messageBundle {};

  enqueueType _stopCondition {};
  enqueueType _handlerStopCondition {};

  unsigned int _stopPort {};

  bool _udp {false};
  bool _stopThread {false};

public:
  /**
   * @brief Constructor for the Listener class.
   * @param queue The _queue to push the data to.
   * @param listeningSocket The socket to listen to.
   * @param messageBundle The message bundle to use.
   * @param stopCondition The condition to stop the listener.
   * @param handlerStopCondition The condition to stop the handler.
   * @param stopPort The port to stop the listener.
   */
  Listener(Queue<enqueueType>* queue,
           std::shared_ptr<SocketType> listeningSocket,
           ListenerMessageBundle& messageBundle,
           enqueueType stopCondition,
           enqueueType handlerStopCondition,
           unsigned int stopPort)
      : _queue(queue)
      , _listeningSocket(std::move(listeningSocket))
      , _messageBundle(std::move(messageBundle))
      , _stopCondition(stopCondition)
      , _handlerStopCondition(handlerStopCondition)
      , _stopPort(stopPort) {
  }

  /**
   * @brief Destructor for the Listener class.
   */
  ~Listener() override = default;

  void stop() {
    this->_stopThread = true;

    this->unlockListen();

    this->_queue->push(_handlerStopCondition);
  }

private:
  /**
   * @brief Sets the socket as UDP.
   */
  virtual void listen() {
    while (true) {
      printf("Listener: %s", this->_messageBundle.listeningMessage.c_str());

      enqueueType data = this->obtain();

      if (data == this->_stopCondition || this->_stopThread) {
        this->_queue->push(data);
        printf("Listener OE: %s", this->_messageBundle.stopMessage.c_str());

        break;
      }

      printf("Listener AR: %s", this->_messageBundle.afterReceivedMessage.c_str());

      this->_queue->push(data);
    }
  }

  /**
   * @brief Runs the listener.
   */
  void run() override {
    this->listen();
  }

  /**
   * @brief Obtains the data from the socket.
   * @return The data obtained.
   */
  virtual enqueueType obtain() = 0;

  /**
   * @brief Unlocks the listening socket.
   */
  void unlockListen() {
    SocketType closingSocket(_listeningSocket->getType());
    closingSocket.connect("any IP", _stopPort);
  }
};