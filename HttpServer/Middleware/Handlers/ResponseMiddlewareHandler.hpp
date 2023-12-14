//
// Created by josephvalverde on 12/11/23.
//

#ifndef LEGO_FIGURE_MAKER_RESPONSEMIDDLEWAREHANDLER_IXX
#define LEGO_FIGURE_MAKER_RESPONSEMIDDLEWAREHANDLER_IXX

#include "Handler.hpp"
#include "Http/HttpMessages/HttpResponse.hpp"

class ResponseMiddlewareHandler : public Handler<std::shared_ptr<HttpResponse>> {
public:
    explicit ResponseMiddlewareHandler(Queue<std::shared_ptr<HttpResponse>>* consumingQueue,
                              std::shared_ptr<HttpResponse> stopCondition);

private:
    void optionalToEnd() override;

    void handleSingle(std::shared_ptr<HttpResponse> handlingData) override;
};


#endif //LEGO_FIGURE_MAKER_RESPONSEMIDDLEWAREHANDLER_IXX
