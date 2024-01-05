//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX

#include "Server/Middleware/Handler.hpp"
#include "../../Http/HttpMessages/Reponse/HttpResponse.hpp"
#include "Server/Protocols/IResponseHeaderComposer.hpp"
#include "../../Protocols/ProtocolConcept.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class ResponseMiddlewareHandler : public Handler<std::shared_ptr<HttpResponse>> {
    using ConsumingType = std::shared_ptr<typename Protocol::ResponseType>;
    using ResponseBodySerializer = typename Protocol::ResponseBodySerializer;
    std::shared_ptr<IResponseHeaderComposer<typename Protocol::ResponseType>> _headerComposer;

    ResponseBodySerializer _bodySerializer;

  public:
    explicit ResponseMiddlewareHandler(MiddlewareBlockingQueue<ConsumingType>& consumingQueue
                                       , std::shared_ptr<IResponseHeaderComposer<typename Protocol::ResponseType>> headerComposer
                                       , std::shared_ptr<ILogger> logger)
        : Handler(consumingQueue, std::move(logger))
        , _headerComposer(std::move(headerComposer))
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

      std::string header = std::move(_headerComposer->composeHeader(*handlingData));

      std::vector<char> responseVector(header.begin(), header.end());

      std::vector<char> body = std::move(_bodySerializer.serialize(*handlingData));

      responseVector.insert(responseVector.end(), body.begin(), body.end());

      *handlingData->socket << responseVector;
    };
  };
}

#endif //HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
