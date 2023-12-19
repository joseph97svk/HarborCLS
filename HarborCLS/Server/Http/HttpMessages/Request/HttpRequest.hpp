//
// Created by josephvalverde on 12/11/23.
//
#ifndef HTTPSERVER_HTTPREQUEST_HPP
#define HTTPSERVER_HTTPREQUEST_HPP

#include "Basic_HttpRequest.hpp"

#include "../../../Socket/TcpSocket.hpp"

namespace HarborCLS {

  using HttpRequest = Basic_HttpRequest<TcpSocket>;
}

#endif //HTTPSERVER_HTTPREQUEST_HPP