//
// Created by josephvalverde on 12/11/23.
//
#include <vector>

#include "Middleware/Handlers/RequestMiddlewareHandler.hpp"
#include "Middleware/Handlers/ApplicationMiddlewareHandler.hpp"
#include "Middleware/Handlers/ResponseMiddlewareHandler.hpp"
#include "Middleware/Listeners/TcpListener.hpp"

#include "Socket/TcpSocket.hpp"
#include "JsonReader/JsonHandler.hpp"
#include "JsonParsingPolicies/ServerConfigurationParsingPolicy.hpp"
#include "Logger/Logger.hpp"


#include "common.hpp"

class HttpServer {
  std::shared_ptr<TcpSocket> _tcpSocket;
  std::shared_ptr<TcpListener> _tcpListener;

  std::vector<RequestMiddlewareHandler> _requestMiddlewareHandlers;
  std::vector<ApplicationMiddlewareHandler> _applicationMiddlewareHandlers;
  std::vector<ResponseMiddlewareHandler> _responseMiddlewareHandlers;

  Queue<std::shared_ptr<TcpSocket>> _connectionsQueue;
  Queue<std::shared_ptr<HttpRequest>> _requestsQueue;
  Queue<std::shared_ptr<HttpResponse>> _responsesQueue;

  ServerConfiguration _configuration;

  std::unique_ptr<ILogger> _logger;

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

