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

    using RequestParserInterface = IRequestParser<SocketType, RequestType>;

    using ConsumingType = std::shared_ptr<SocketType>;
    using ProducingType = std::shared_ptr<RequestType>;

    MiddlewareBlockingQueue<ProducingType>& _requestsQueue;

    std::shared_ptr<RequestParserInterface> _requestParser;

  public:
    /**
     * @brief constructor
     * @param consumingQueue the queue that the handler is going to consume from.
     * @param producingQueue the queue that the handler is going to produce to.
     * @param stopCondition the condition that the handler is going to stop on.
     */
    RequestMiddlewareHandler(MiddlewareBlockingQueue<ConsumingType>& consumingQueue
                             , MiddlewareBlockingQueue<ProducingType>& producingQueue
                             , std::shared_ptr<RequestParserInterface> requestParser
                             , std::shared_ptr<ILogger> logger)
        : Handler(consumingQueue, std::move(logger))
        , _requestsQueue(producingQueue)
        , _requestParser(std::move(requestParser)) {
      _id = "RequestMiddlewareHandler: ";
    }

  private:
    /**
     * This method is called when the handler is about to end its operation.
     */
    void optionalToEnd() override {
      _requestsQueue.push(MiddlewareMessage<ProducingType>(StopCondition()));
    }

    /**
     * @brief gets socket from received connection and adds processed http request to queue
     * @param handlingData the data that is going to be handled.
     */
    void handleSingle(ConsumingType handlingData) override {
      std::vector<char> requestData;

      *handlingData >> requestData;

      // ignore empty requests
      if (std::all_of(requestData.begin(), requestData.end(), [](char c) { return c == '\0'; })) {
        return;
      }

      std::string_view requestString(requestData.begin(), requestData.end());
      std::cout << "request text with size {" << requestString.size() << "}: " << std::endl;
      std::cout << requestString << std::endl;

      ProducingType request = _requestParser->createRequest(requestData, handlingData);

      _requestsQueue.push(MiddlewareMessage<ProducingType>(request));
    };
  };
}

#endif //HARBOR_CLS_REQUESTMIDDLEWAREHANDLER_IXX
