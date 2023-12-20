//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX

namespace HarborCLS {

  template<typename Protocol>
  class ApplicationMiddlewareHandler
      : public Handler<std::shared_ptr<typename Protocol::RequestType>> {
    using RequestType = typename Protocol::RequestType;
    using ResponseType = typename Protocol::ResponseType;

    Queue<std::shared_ptr<ResponseType>> &responsesQueue;

  public:
    ApplicationMiddlewareHandler(Queue<std::shared_ptr<RequestType>> *consumingQueue,
                                 Queue<std::shared_ptr<ResponseType>> &producingQueue,
                                 std::shared_ptr<RequestType> stopCondition)
        : Handler<std::shared_ptr<RequestType>>(consumingQueue, stopCondition), responsesQueue(producingQueue) {}

  private:
    void optionalToEnd() override {

    }

    void handleSingle(std::shared_ptr<RequestType> handlingData) override {
      std::shared_ptr<HttpResponse> response = std::make_shared<HttpResponse>();
      response->socket = handlingData->socket;

      std::string body = "<html><body><h1>Hello there</h1></body></html>";

      response->body = body;
      response->contentLength = body.length();

      responsesQueue.push(response);
    };
  };
}

#endif //HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX
