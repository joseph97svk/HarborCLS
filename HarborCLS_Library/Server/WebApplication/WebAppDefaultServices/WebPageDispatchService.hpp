//
// Created by josephvalverde on 12/24/23.
//

#ifndef HARBOR_CLS_WEBPAGEDISPATCHSERVICE_HPP
#define HARBOR_CLS_WEBPAGEDISPATCHSERVICE_HPP

#include "../DependencyManagement/BaseWebAppService.hpp"
#include "Server/Http/HttpMessages/Reponse/HttpResponse.hpp"

namespace HarborCLS {

  template<ServerProtocol Protocol>
  class WebServiceDispatchService : public BaseWebAppService<Protocol> {
    using ResponseType = typename Protocol::ResponseType;

  public:
    void release() override {
      // no special loop to be released is present, so nothing to do here
    }

    bool canHandle(std::shared_ptr<typename Protocol::RequestType> request) override {
      return false;
    }

    MiddlewareMessage<std::shared_ptr<typename Protocol::ResponseType>> handleTask(
        std::shared_ptr<typename Protocol::RequestType> request) override {
      std::shared_ptr<HttpResponse> response = std::make_shared<HttpResponse>();
      response->socket = request->socket;
      std::string body = "<html><body><h1>Sup there</h1></body></html>";


      response->body = body;
      response->contentLength = body.length();


      return MiddlewareMessage<std::shared_ptr<ResponseType>>(response);
    }
  };
}

#endif //HARBOR_CLS_WEBPAGEDISPATCHSERVICE_HPP
