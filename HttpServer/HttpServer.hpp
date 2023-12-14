//
// Created by josephvalverde on 12/11/23.
//
#include <vector>

#include "Middleware/Handlers/RequestMiddlewareHandler.hpp"
#include "Middleware/Handlers/ApplicationMiddlewareHandler.hpp"
#include "Middleware/Handlers/ResponseMiddlewareHandler.hpp"
#include "Middleware/Listeners/TcpListener.hpp"
/*#include "Http/HttpMessages/HttpRequest.hpp"
#include "Http/HttpMessages/HttpResponse.hpp"
#include "ServerConfiguration.hpp"
#include "Middleware/ListenerMessageBundle.hpp"*/
#include "Socket/TcpSocket.hpp"
#include "JsonReader/JsonHandler.hpp"
#include "JsonParsingPolicies/ServerConfigurationParsingPolicy.hpp"

#include "common.hpp"

class HttpServer {
  std::shared_ptr<TcpSocket> tcpSocket;
  std::shared_ptr<TcpListener> tcpListener;

  std::vector<RequestMiddlewareHandler> requestMiddlewareHandlers;
  std::vector<ApplicationMiddlewareHandler> applicationMiddlewareHandlers;
  std::vector<ResponseMiddlewareHandler> responseMiddlewareHandlers;

  Queue<std::shared_ptr<TcpSocket>> connectionsQueue;
  Queue<std::shared_ptr<HttpRequest>> requestsQueue;
  Queue<std::shared_ptr<HttpResponse>> responsesQueue;

  ServerConfiguration configuration;

public:
    static HttpServer& getInstance();

    NO_COPY(HttpServer)

    void addConfiguration(const std::string& configurationJsonPath);

    [[noreturn]] void startServer();

    [[noreturn]] void stopServer();

protected:
    void setUpServer();

    HttpServer() = default;
};

