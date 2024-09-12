//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_BASEWEBAPPSERVICE_HPP
#define HARBOR_CLS_BASEWEBAPPSERVICE_HPP

#include <functional>
#include <iostream>

#include "Server/Concurrency/Thread.hpp"
#include "Server/Middleware/MiddlewareMessage.hpp"
#include "Logger/ILogger.hpp"
#include "../../Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class BaseWebAppService : public Thread {
    using IncomingMessageType = std::shared_ptr<typename Protocol::RequestType>;
    using OutgoingMessageType = std::shared_ptr<typename Protocol::ResponseType>;

  protected:
    MiddlewareBlockingQueue<IncomingMessageType> _entryQueue {};

    std::optional<std::reference_wrapper<MiddlewareBlockingQueue<OutgoingMessageType>>> _exitQueue { std::nullopt };

    std::optional<std::function<void(BaseWebAppService&)>> _setUpSequence { std::nullopt };
    std::optional<std::function<void(BaseWebAppService&)>> _tearDownSequence { std::nullopt };

    std::shared_ptr<ILogger> _logger {};
  public:
    void setWebAppLinking(std::reference_wrapper<MiddlewareBlockingQueue<OutgoingMessageType>> exitQueue
                          , std::shared_ptr<ILogger> logger) {
      _exitQueue = exitQueue;
      _logger = std::move(logger);
    }

    virtual bool canHandle(IncomingMessageType request) = 0;

    MiddlewareBlockingQueue<IncomingMessageType>& getEntryQueue() {
      return _entryQueue;
    }

    void setSetUpSequence(std::function<void(BaseWebAppService&)> setUpSequence) {
      _setUpSequence = std::move(setUpSequence);
    }

    void setTearDownSequence(std::function<void(BaseWebAppService&)> tearDownSequence) {
      _tearDownSequence = std::move(tearDownSequence);
    }

    std::shared_ptr<ILogger> getLogger() {
      return _logger;
    }

  protected:
    void run() override {
      if (_setUpSequence) {
        _setUpSequence.value()(*this);
      }

      while (true) {
        MiddlewareMessage<IncomingMessageType> data = _entryQueue.pop();

        std::expected<IncomingMessageType, std::variant<StopCondition, Error<MessageErrors>>> content
            = data.getContent();

        if (content) {
          std::optional<MiddlewareMessage<OutgoingMessageType>> result = handleTask(content.value());

          if (result) {
            _exitQueue.value().get().push(*result);
          }

          continue;
        }

        std::visit(overloaded{
            [this](StopCondition &stopCondition) {
              _exitQueue.value().get().push(MiddlewareMessage<OutgoingMessageType>(stopCondition));
            },
            [this](Error<MessageErrors> &error) {
              _logger->error(error.what());
            }
        }, content.error());

        if (_tearDownSequence) {
          _tearDownSequence.value()(*this);
        }

        break;
      }
    }

    [[nodiscard]] virtual std::optional<MiddlewareMessage<OutgoingMessageType>> handleTask(IncomingMessageType message) = 0;

    void byPass() {
      _entryQueue.push(MiddlewareMessage<IncomingMessageType>(IncomingMessageType()));
    }
  };

}

#endif //HARBOR_CLS_BASEWEBAPPSERVICE_HPP
