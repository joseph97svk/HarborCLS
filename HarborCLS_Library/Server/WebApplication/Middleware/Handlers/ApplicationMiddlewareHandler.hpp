//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX

#include "../../../Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class ApplicationMiddlewareHandler
      : public Handler<std::shared_ptr<typename Protocol::RequestType>> {
    using ConsumingType = std::shared_ptr<typename Protocol::RequestType>;
    using ProducingType = std::shared_ptr<typename Protocol::ResponseType>;

    MiddlewareBlockingQueue<ProducingType> &_responsesQueue;

  public:
    ApplicationMiddlewareHandler(MiddlewareBlockingQueue<ConsumingType>& consumingQueue,
                                 MiddlewareBlockingQueue<ProducingType>& producingQueue)
        : Handler<ConsumingType>(consumingQueue)
            , _responsesQueue(producingQueue) {}

  private:
    void optionalToEnd() override {
      _responsesQueue.push(MiddlewareMessage<ProducingType>(StopCondition()));
    }

    void handleSingle(ConsumingType handlingData) override {
      std::shared_ptr<HttpResponse> response = std::make_shared<HttpResponse>();
      response->socket = handlingData->socket;

      std::string body = "<html><body><h1>Hello there</h1></body></html>";

      response->body = body;
      response->contentLength = body.length();

      _responsesQueue.push(MiddlewareMessage<ProducingType>(response));
    };
  };
}

#endif //HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX
