//
// Created by josephvalverde on 12/11/23.
//

#ifndef LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX
#define LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX

#include <utility>

#include "BaseElements/Handler.hpp"
#include "Http/HttpMessages/Request/HttpRequest.hpp"
#include "Socket/TcpSocket.hpp"
#include "Http/HttpRequestParser/IHttpRequestParser.hpp"

class RequestMiddlewareHandler : public Handler<std::shared_ptr<TcpSocket>> {
  Queue<std::shared_ptr<HttpRequest>>& _requestsQueue;

  std::shared_ptr<IHttpRequestParser<TcpSocket>> _requestParser;

public:
  /**
   * @brief constructor
   * @param consumingQueue the queue that the handler is going to consume from.
   * @param producingQueue the queue that the handler is going to produce to.
   * @param stopCondition the condition that the handler is going to stop on.
   */
  RequestMiddlewareHandler(Queue<std::shared_ptr<TcpSocket>>* consumingQueue,
                           Queue<std::shared_ptr<HttpRequest>>& producingQueue,
                           std::shared_ptr<TcpSocket> stopCondition,
                           std::shared_ptr<IHttpRequestParser<TcpSocket>> requestParser);

private:
  /**
   * This method is called when the handler is about to end its operation.
   */
  void optionalToEnd() override;

  /**
   * @brief gets socket from received connection and adds processed http request to queue
   * @param handlingData the data that is going to be handled.
   */
  void handleSingle(std::shared_ptr<TcpSocket> handlingData) override;
};


#endif //LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX
