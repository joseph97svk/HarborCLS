//
// Created by josephvalverde on 12/11/23.
//

#ifndef HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX
#define HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX

#include "../../../Protocols/ProtocolConcept.hpp"

#include "../../DependencyManagement/BaseWebAppService.hpp"
#include "Server/WebApplication/DependencyManagement/IWebServiceResolver.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class ApplicationMiddlewareHandler
      : public Handler<std::shared_ptr<typename Protocol::RequestType>> {
    using ConsumingType = std::shared_ptr<typename Protocol::RequestType>;
    using ProducingType = std::shared_ptr<typename Protocol::ResponseType>;

    MiddlewareBlockingQueue<ProducingType>& _responsesQueue;
    std::shared_ptr<std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>> _services;
    std::shared_ptr<IWebServiceResolver<Protocol>> _webServiceResolver;

  public:
    ApplicationMiddlewareHandler(MiddlewareBlockingQueue<ConsumingType>& consumingQueue
                                 , MiddlewareBlockingQueue<ProducingType>& producingQueue
                                 , std::shared_ptr<ILogger> logger
                                 , std::shared_ptr<std::vector<std::shared_ptr<BaseWebAppService<Protocol>>>> services
                                 , std::shared_ptr<IWebServiceResolver<Protocol>> webServiceResolver)
        : Handler<ConsumingType>(consumingQueue, std::move(logger))
            , _responsesQueue(producingQueue)
            , _services(services)
            , _webServiceResolver(webServiceResolver) {
      this->_id = "ApplicationMiddlewareHandler: ";
    }

  private:
    void optionalToEnd() override {
      for (auto& service : *_services) {
        service->getEntryQueue().push(StopCondition());
        service->waitToFinish();
      }

      _responsesQueue.push(MiddlewareMessage<ProducingType>(StopCondition()));
    }

    void handleSingle(ConsumingType handlingData) override {
      std::shared_ptr<BaseWebAppService<Protocol>> selectedApp =
          _webServiceResolver->resolve(handlingData, *_services);

      if (selectedApp == nullptr) {
        _responsesQueue.push(MiddlewareMessage<ProducingType>(Error<MessageErrors>(
            "No registered services found for web application"
            , MessageErrors::GENERIC_ERROR)));
      }

      selectedApp->getEntryQueue().push(MiddlewareMessage<ConsumingType>(std::move(handlingData)));
    };
  };
}

#endif //HARBOR_CLS_APPLICATIONMIDDLEWAREHANDLER_IXX
