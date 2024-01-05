//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_HTTPBEHAVIOR_HPP
#define HARBOR_CLS_HTTPBEHAVIOR_HPP

namespace HarborCLS {

  template<typename Protocol = HttpProtocol>
  struct HttpBehavior {
    using SocketType = typename Protocol::SocketType;
    using RequestType = typename Protocol::RequestType;

    std::shared_ptr<IRequestParser<SocketType, RequestType>> requestParser;
    std::shared_ptr<IResponseHeaderComposer<typename Protocol::ResponseType>> responseHeaderComposer;
  };
}

#endif //HARBOR_CLS_HTTPBEHAVIOR_HPP
