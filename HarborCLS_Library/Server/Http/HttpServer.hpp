//
// Created by josephvalverde on 12/19/23.
//

#ifndef HARBOR_CLS_HTTPSERVER_HPP
#define HARBOR_CLS_HTTPSERVER_HPP

#include "../GenericServer.hpp"

namespace HarborCLS {
  using HttpServer = GenericServer<HttpProtocol>;
}

#endif //HARBOR_CLS_HTTPSERVER_HPP
