//
// Created by josephvalverde on 12/18/23.
//

#ifndef HARBOR_CLS_SERVERBEHAVIOR_HPP
#define HARBOR_CLS_SERVERBEHAVIOR_HPP

namespace HarborCLS {

  template<typename Protocol = HttpProtocol>
  struct ServerBehavior {
    using SocketType = typename Protocol::SocketType;
    using RequestType = typename Protocol::RequestType;

    std::shared_ptr<IRequestParser<SocketType, RequestType>> requestParser;
    std::shared_ptr<IResponseComposer<typename Protocol::ResponseType>> responseHeaderComposer;
  };
}

#endif //HARBOR_CLS_SERVERBEHAVIOR_HPP
