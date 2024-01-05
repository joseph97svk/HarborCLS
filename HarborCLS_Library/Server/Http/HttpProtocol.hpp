//
// Created by josephvalverde on 12/19/23.
//

#ifndef HARBOR_CLS_HTTPPROTOCOL_HPP
#define HARBOR_CLS_HTTPPROTOCOL_HPP

#include "Server/Http/HttpMessages/Request/HttpRequest.hpp"
#include "Server/Http/HttpRequestParser/HttpRequestParser.hpp"
#include "Server/Protocols/IRequestParser.hpp"
#include "Server/Http/ResponseSerialization/HttpResponseComposer.hpp"

namespace HarborCLS {

  struct HttpProtocol {
    using RequestType = HttpRequest;
    using ResponseType = HttpResponse;
    using SocketType = TcpSocket;
    using RequestParserType = HttpRequestParser;
    using ResponseComposerType = HttpResponseComposer;
  };

}

#endif //HARBOR_CLS_HTTPPROTOCOL_HPP
