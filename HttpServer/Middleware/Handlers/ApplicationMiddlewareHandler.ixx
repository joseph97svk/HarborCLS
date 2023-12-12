//
// Created by josephvalverde on 12/11/23.
//

#ifndef LEGO_FIGURE_MAKER_APPLICATIONMIDDLEWAREHANDLER_IXX
#define LEGO_FIGURE_MAKER_APPLICATIONMIDDLEWAREHANDLER_IXX

#include "Handler.hpp"

export module ApplicationMiddleware;

import HttpRequest;
import HttpResponse;

export class ApplicationMiddlewareHandler : public Handler<std::shared_ptr<HttpRequest>> {
    Queue<std::shared_ptr<HttpResponse>>& responsesQueue;

public:
    ApplicationMiddlewareHandler(Queue<std::shared_ptr<HttpRequest>>* consumingQueue,
                                 Queue<std::shared_ptr<HttpResponse>>& producingQueue,
                                 std::shared_ptr<HttpRequest> stopCondition);

private:
    void optionalToEnd() override;

    void handleSingle(std::shared_ptr<HttpRequest> handlingData) override;
};


#endif //LEGO_FIGURE_MAKER_APPLICATIONMIDDLEWAREHANDLER_IXX
