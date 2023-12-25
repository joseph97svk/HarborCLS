//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX

#include "Server/Middleware/Handler.hpp"
#include "../../Http/HttpMessages/Reponse/HttpResponse.hpp"
#include "../../Http/ResponseHeaderComposer/IResponseHeaderComposer.hpp"
#include "../../Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class ResponseMiddlewareHandler : public Handler<std::shared_ptr<HttpResponse>> {
    using ConsumingType = std::shared_ptr<typename Protocol::ResponseType>;

    std::shared_ptr<IResponseHeaderComposer> _headerComposer;

  public:
    explicit ResponseMiddlewareHandler(MiddlewareBlockingQueue<ConsumingType>& consumingQueue
                                       , std::shared_ptr<IResponseHeaderComposer> headerComposer
                                       , std::shared_ptr<ILogger> logger)
        : Handler(consumingQueue, std::move(logger))
        , _headerComposer(std::move(headerComposer))
    {}

  private:
    void optionalToEnd() override {

    }

    void handleSingle(ConsumingType handlingData) override{
      std::string header = std::move(_headerComposer->composeHeader(handlingData));

      std::vector<char> responseVector(header.begin(), header.end());

      std::visit(overloaded{
          [&responseVector](std::string &str) {
            responseVector.insert(responseVector.end(), str.begin(), str.end());
          },
          [&responseVector](std::vector<char> &vec) {
            responseVector.insert(responseVector.end(), vec.begin(), vec.end());
          }
      }, handlingData->body);

      *handlingData->socket << responseVector;
    };
  };
}

#endif //HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
