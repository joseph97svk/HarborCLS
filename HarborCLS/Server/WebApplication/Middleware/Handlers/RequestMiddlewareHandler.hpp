//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_REQUESTMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_REQUESTMIDDLEWAREHANDLER_IXX

#include <utility>

#include "../../../Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class RequestMiddlewareHandler : public Handler<std::shared_ptr<TcpSocket>> {
    using RequestType = typename Protocol::RequestType;
    using SocketType = typename Protocol::SocketType;

    Queue<std::shared_ptr<RequestType>> &_requestsQueue;

    std::shared_ptr<IRequestParser<SocketType, RequestType>> _requestParser;

  public:
    /**
     * @brief constructor
     * @param consumingQueue the queue that the handler is going to consume from.
     * @param producingQueue the queue that the handler is going to produce to.
     * @param stopCondition the condition that the handler is going to stop on.
     */
    RequestMiddlewareHandler(Queue<std::shared_ptr<SocketType>> *consumingQueue,
                             Queue<std::shared_ptr<HttpRequest>> &producingQueue,
                             std::shared_ptr<SocketType> stopCondition,
                             std::shared_ptr<IRequestParser<SocketType, RequestType>> requestParser)
        : Handler(consumingQueue, std::move(stopCondition)), _requestsQueue(producingQueue),
          _requestParser(std::move(requestParser)) {}

  private:
    /**
     * This method is called when the handler is about to end its operation.
     */
    void optionalToEnd() override {

    }

    /**
     * @brief gets socket from received connection and adds processed http request to queue
     * @param handlingData the data that is going to be handled.
     */
    void handleSingle(std::shared_ptr<SocketType> handlingData) override {
      std::vector<char> request;

      *handlingData >> request;

      std::shared_ptr<RequestType> httpRequest = _requestParser->createHttpRequest(request, handlingData);

      _requestsQueue.push(httpRequest);
    };
  };
}

#endif //HARBOR_CLS_REQUESTMIDDLEWAREHANDLER_IXX
