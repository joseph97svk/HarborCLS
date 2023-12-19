//
// Created by josephvalverde on 12/19/23.
//

#ifndef HARBOR_CLS_HTTPPROTOCOL_HPP
#define HARBOR_CLS_HTTPPROTOCOL_HPP

#include "../Http/HttpMessages/Request/HttpRequest.hpp"
#include "../Http/HttpRequestParser/HttpRequestParser.hpp"
#include "../Http/HttpRequestParser/IRequestParser.hpp"
#include "Server/Http/ResponseHeaderComposer/HttpResponseHeaderComposer.hpp"

namespace HarborCLS {

  struct HttpProtocol {
    using RequestType = HttpRequest;
    using ResponseType = HttpResponse;
    using SocketType = TcpSocket;
    using RequestParserInterface = IRequestParser<SocketType, RequestType>;
    using RequestParserType = HttpRequestParser;
    using ResponseHeaderComposerInterface = IResponseHeaderComposer;
    using ResponseHeaderComposerType = HttpResponseHeaderComposer;
  };
}
#endif //HARBOR_CLS_HTTPPROTOCOL_HPP
