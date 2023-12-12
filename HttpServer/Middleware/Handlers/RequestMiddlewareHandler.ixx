//
// Created by josephvalverde on 12/11/23.
//

#ifndef LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX
#define LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX

#include <utility>

#include "Handler.hpp"

export module RequestMiddleware;

import HttpRequest;

export class RequestMiddlewareHandler : public Handler<std::shared_ptr<Socket>> {
    Queue<std::shared_ptr<HttpRequest>>& requestsQueue;

public:
    RequestMiddlewareHandler(Queue<std::shared_ptr<Socket>>* consumingQueue,
                             Queue<std::shared_ptr<HttpRequest>>& producingQueue,
                             std::shared_ptr<Socket> stopCondition);

private:
    void optionalToEnd() override;

    void handleSingle(std::shared_ptr<Socket> handlingData) override;
};


#endif //LEGO_FIGURE_MAKER_REQUESTMIDDLEWAREHANDLER_IXX
