//
// Created by josephvalverde on 12/11/23.
//

#ifndef LEGO_FIGURE_MAKER_HTTPSERVER_H
#define LEGO_FIGURE_MAKER_HTTPSERVER_H

#include <vector>
#include "Handler.hpp"

export module HttpServer;

import RequestMiddleware;
import ApplicationMiddleware;
import ResponseMiddleware;
import TcpListener;
import HttpRequest;
import HttpResponse;
import ServerConfiguration;
import ListenerMessageBundle;

export class HttpServer {
  std::shared_ptr<Socket> tcpSocket;
  std::shared_ptr<TcpListener> tcpListener;

  std::vector<RequestMiddlewareHandler> requestMiddlewareHandlers;
  std::vector<ApplicationMiddlewareHandler> applicationMiddlewareHandlers;
  std::vector<ResponseMiddlewareHandler> responseMiddlewareHandlers;

  Queue<std::shared_ptr<Socket>> connectionsQueue;
  Queue<std::shared_ptr<HttpRequest>> requestsQueue;
  Queue<std::shared_ptr<HttpResponse>> responsesQueue;

  ServerConfiguration configuration;

public:
    static HttpServer& getInstance();

    HttpServer(HttpServer&&) = delete;
    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(HttpServer&&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    void addConfiguration(const std::string& configurationJsonPath);

    [[noreturn]] void startServer();

    [[noreturn]] void stopServer();

protected:
    void setUpServer();

    HttpServer() = default;
};


#endif //LEGO_FIGURE_MAKER_HTTPSERVER_H
