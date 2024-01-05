//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX

#include "Server/Middleware/Handler.hpp"
#include "../../Http/HttpMessages/Reponse/HttpResponse.hpp"
#include "Server/Protocols/IResponseComposer.hpp"
#include "../../Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class ResponseMiddlewareHandler : public Handler<std::shared_ptr<HttpResponse>> {
    using ConsumingType = std::shared_ptr<typename Protocol::ResponseType>;
    std::shared_ptr<IResponseComposer<typename Protocol::ResponseType>> _responseComposer;

  public:
    explicit ResponseMiddlewareHandler(MiddlewareBlockingQueue<ConsumingType>& consumingQueue
                                       , std::shared_ptr<IResponseComposer<typename Protocol::ResponseType>> responseComposer
                                       , std::shared_ptr<ILogger> logger)
        : Handler(consumingQueue, std::move(logger))
        , _responseComposer(std::move(responseComposer))
    {}

  private:
    void optionalToEnd() override {

    }

    void handleSingle(ConsumingType handlingData) override{
      if (handlingData == nullptr) {
        _logger->error(
            "Handling data passed as response is null. Either do not respond or pass a valid response."
            );

        return;
      }

      std::vector<char> responseVector = std::move(_responseComposer->compose(*handlingData));

      *handlingData->socket << responseVector;
    };
  };
}

#endif //HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
