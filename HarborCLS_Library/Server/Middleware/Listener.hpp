//
// Created by josephvalverde on 12/12/23.
//

#ifndef HARBOR_CLS_LISTENER_HPP
#define HARBOR_CLS_LISTENER_HPP

#include <memory>

#include "Server/Concurrency/Thread.hpp"
#include "Server/Concurrency/Queue.hpp"
#include "MiddlewareMessage.hpp"
#include "../../Logger/ILogger.hpp"

namespace HarborCLS {

  template<typename enqueueType, typename SocketType>
  class Listener : public virtual Thread {
  protected:
    // still have no idea why its required here to work but not anywhere else
    std::reference_wrapper<MiddlewareBlockingQueue<enqueueType>> _queue;
    std::shared_ptr<SocketType> _listeningSocket{};

    unsigned int _stopPort{};
    bool _stopThread{ false };

    std::shared_ptr<ILogger> _logger;
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
    Listener(MiddlewareBlockingQueue<enqueueType>& queue,
             std::shared_ptr<SocketType> listeningSocket,
             unsigned int stopPort,
             std::shared_ptr<ILogger> logger)
        : _queue(queue)
        , _listeningSocket(std::move(listeningSocket))
        , _stopPort(stopPort)
        , _logger(std::move(logger)){
    }

    /**
     * @brief Destructor for the Listener class.
     */
    ~Listener() override = default;

    void stop() {
      _stopThread = true;

      this->unlockListen();

      _queue.get().push(MiddlewareMessage<enqueueType>(StopCondition()));
    }

  private:
    /**
     * @brief Sets the socket as UDP.
     */
    virtual void listen() {
      while (true) {
        MiddlewareMessage<enqueueType> data = this->obtain();

        if (_stopThread) {
          _queue.get().push(MiddlewareMessage<enqueueType>(StopCondition()));
          break;
        }

        std::expected<enqueueType, std::variant<StopCondition, Error<MessageErrors>>> dataContents = data.getContent();
        if (!dataContents) {
          std::visit(overloaded{
              [this](StopCondition& stopCondition) {
                _stopThread = true;
              },
              [this](Error<MessageErrors>& error) {
                std::string errorMessage = error;
                _logger->error("Error while obtaining data: " + errorMessage);
                _queue.get().push(MiddlewareMessage<enqueueType>(error));
              }
          }, dataContents.error());

          if (_stopThread) {
            _queue.get().push(MiddlewareMessage<enqueueType>(StopCondition()));
            break;
          }
        }

        _queue.get().push(data);
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
    virtual MiddlewareMessage<enqueueType> obtain() = 0;

    /**
     * @brief Unlocks the listening socket.
     */
    void unlockListen() {
      SocketType closingSocket(_listeningSocket->isIpV6());
      std::expected<Success, SocketError> socketConnection = closingSocket.connect("any IP", _stopPort);

      if (!socketConnection) {
        std::string errorMessage = socketConnection.error();
        _logger->error("Error connecting to stop port: " + errorMessage);
        _logger->error("Could not exit gracefully, will force exit. May need to kill ports manually.");
        exit(0);
      }
    }
  };
}

#endif //HARBOR_CLS_LISTENER_HPP