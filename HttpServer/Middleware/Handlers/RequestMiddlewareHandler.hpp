//
// Created by josephvalverde on 12/11/23.
//

#ifndef LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX
#define LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX

#include <utility>

#include "Handler.hpp"
#include "Http/HttpMessages/HttpRequest.hpp"
#include "Socket/TcpSocket.hpp"

class RequestMiddlewareHandler : public Handler<std::shared_ptr<TcpSocket>> {
    Queue<std::shared_ptr<HttpRequest>>& requestsQueue;

public:
    RequestMiddlewareHandler(Queue<std::shared_ptr<TcpSocket>>* consumingQueue,
                             Queue<std::shared_ptr<HttpRequest>>& producingQueue,
                             std::shared_ptr<TcpSocket> stopCondition);

    RequestMiddlewareHandler()
    : Handler(nullptr, nullptr),
      requestsQueue(*new Queue<std::shared_ptr<HttpRequest>>()) {

    }
private:
    void optionalToEnd() override;

    void handleSingle(std::shared_ptr<TcpSocket> handlingData) override;
};


#endif //LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX
