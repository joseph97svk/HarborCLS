//
// Created by josephvalverde on 12/11/23.
//

#ifndef LEGO_FIGURE_MAKER_RESPONSEMIDDLEWAREHANDLER_IXX
#define LEGO_FIGURE_MAKER_RESPONSEMIDDLEWAREHANDLER_IXX

#include "BaseElements/Handler.hpp"
#include "Http/HttpMessages/Reponse/HttpResponse.hpp"
#include "Http/ResponseHeaderComposer/IResponseHeaderComposer.hpp"

class ResponseMiddlewareHandler : public Handler<std::shared_ptr<HttpResponse>> {
  std::shared_ptr<IResponseHeaderComposer> _headerComposer;

public:
    explicit ResponseMiddlewareHandler(Queue<std::shared_ptr<HttpResponse>>* consumingQueue,
                              std::shared_ptr<HttpResponse> stopCondition,
                              std::shared_ptr<IResponseHeaderComposer> headerComposer);

private:
    void optionalToEnd() override;

    void handleSingle(std::shared_ptr<HttpResponse> handlingData) override;
};


#endif //LEGO_FIGURE_MAKER_RESPONSEMIDDLEWAREHANDLER_IXX
