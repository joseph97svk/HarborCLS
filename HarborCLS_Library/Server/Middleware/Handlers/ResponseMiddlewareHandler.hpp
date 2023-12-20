//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX

#include "../../BaseElements/Handler.hpp"
#include "../../Http/HttpMessages/Reponse/HttpResponse.hpp"
#include "../../Http/ResponseHeaderComposer/IResponseHeaderComposer.hpp"

namespace HarborCLS {

  class ResponseMiddlewareHandler : public Handler<std::shared_ptr<HttpResponse>> {
    std::shared_ptr<IResponseHeaderComposer> _headerComposer;

  public:
    explicit ResponseMiddlewareHandler(Queue<std::shared_ptr<HttpResponse>> *consumingQueue,
                                       std::shared_ptr<HttpResponse> stopCondition,
                                       std::shared_ptr<IResponseHeaderComposer> headerComposer);

  private:
    void optionalToEnd() override;

    void handleSingle(std::shared_ptr<HttpResponse> handlingData) override;
  };
}

#endif //HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
