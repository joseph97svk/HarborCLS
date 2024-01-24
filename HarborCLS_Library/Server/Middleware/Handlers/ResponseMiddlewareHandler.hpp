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
  class ResponseMiddlewareHandler : public Handler<std::shared_ptr<typename Protocol::ResponseType>> {
    using ConsumingType = std::shared_ptr<typename Protocol::ResponseType>;
    using ComposerType = IResponseComposer<typename Protocol::ResponseType>;

    std::shared_ptr<ComposerType> _responseComposer;

  public:
    explicit ResponseMiddlewareHandler(MiddlewareBlockingQueue<ConsumingType>& consumingQueue
                                       , std::shared_ptr<ComposerType> responseComposer
                                       , std::shared_ptr<ILogger> logger)
        : Handler <std::shared_ptr<typename Protocol::ResponseType>> (consumingQueue, std::move(logger))
        , _responseComposer(std::move(responseComposer))
    {
      this->_id = "ResponseMiddlewareHandler: ";
    }

  private:
    void optionalToEnd() override {

    }

    void handleSingle(ConsumingType handlingData) override {
      if (handlingData == nullptr) {
        this->_logger->error(
            "Handling data passed as response is null. Either do not respond or pass a valid response."
            );

        return;
      }

      std::vector<char> responseVector = _responseComposer->compose(*handlingData);
      std::string message(responseVector.begin(), responseVector.end());

      *handlingData->socket << responseVector;
    };
  };
}

#endif //HARBOR_CLS_RESPONSEMIDDLEWAREHANDLER_IXX
