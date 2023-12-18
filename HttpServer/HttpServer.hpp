//
// Created by josephvalverde on 12/11/23.
//
#include <vector>

#include "Middleware/Handlers/ResponseMiddlewareHandler.hpp"
#include "Logger/Logger.hpp"
#include "WebApplication/WebApplication.hpp"

#include "Common/common.hpp"
#include "Http/HttpBehavior/HttpBehavior.hpp"

class HttpServer {
  std::vector<ResponseMiddlewareHandler> _responseMiddlewareHandlers;
  std::vector<std::reference_wrapper<WebApplication>> _webApplications;

  Queue<std::shared_ptr<HttpResponse>> _responsesQueue;

  ServerConfiguration _configuration;

  std::unique_ptr<ILogger> _logger;

  std::shared_ptr<IHttpRequestParser<TcpSocket>> _requestParser;
  std::shared_ptr<IResponseHeaderComposer> _responseHeaderComposer;
public:
  /*
   * @brief returns the instance of the server
   * @return reference to the server instance
   */
  [[nodiscard]] static HttpServer& getInstance();

  NO_COPY(HttpServer)

  /**
   * @brief adds a web application to the server
   * @param webApplication
   */
  void addWebApplication(WebApplication& webApplication);

  /**
   * @brief sets the behaviour on how http requests and responses are handled
   * @param configuration instance of HttpBehavior with the desired behaviour
   */
  void setHttpBehaviour(HttpBehavior& httpBehavior);

  /**
   * Initializes the server and starts the execution of added web applications
   */
  void startServer();

  /***
   * Stops the operation of the server
   */
  void stopServer();


protected:
  /**
   * Default constructor
   */
  HttpServer() = default;
};

