//
// Created by josephvalverde on 12/21/23.
//

#ifndef HARBOR_CLS_BASEWEBAPPSERVICE_HPP
#define HARBOR_CLS_BASEWEBAPPSERVICE_HPP

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

    std::optional<std::reference_wrapper<MiddlewareBlockingQueue<OutgoingMessageType>>> _exitQueue {};

    std::shared_ptr<ILogger> _logger {};
  public:
    void setWebAppLinking(std::reference_wrapper<MiddlewareBlockingQueue<OutgoingMessageType>> exitQueue
                          , std::shared_ptr<ILogger> logger) {
      _exitQueue = exitQueue;
      _logger = std::move(logger);
    }

    virtual void release() = 0;

    virtual bool canHandle(IncomingMessageType request) = 0;

    MiddlewareBlockingQueue<IncomingMessageType>& getEntryQueue() {
      return _entryQueue;
    }

  protected:
    void run() override {
      while (true) {
        MiddlewareMessage<IncomingMessageType> data = _entryQueue.pop();

        std::expected<IncomingMessageType, std::variant<StopCondition, Error<MessageErrors>>> content
            = data.getContent();

        if (content) {
          MiddlewareMessage<OutgoingMessageType> result = handleTask(content.value());
          _exitQueue.value().get().push(result);
          continue;
        }

        std::visit(overloaded{
            [this](StopCondition &stopCondition) {
              _exitQueue.value().get().push(MiddlewareMessage<OutgoingMessageType>(StopCondition()));
            },
            [this](Error<MessageErrors> &error) {
            }
        }, content.error());

        break;
      }
    }

    [[nodiscard]] virtual MiddlewareMessage<OutgoingMessageType> handleTask(IncomingMessageType message) = 0;

    void byPass() {
      _entryQueue.push(MiddlewareMessage<IncomingMessageType>(IncomingMessageType()));
    }
  };

}

#endif //HARBOR_CLS_BASEWEBAPPSERVICE_HPP
