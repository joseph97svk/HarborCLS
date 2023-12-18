//
// Created by josephvalverde on 12/11/23.
//
#include <vector>

#include "Middleware/Handlers/ResponseMiddlewareHandler.hpp"
#include "Logger/Logger.hpp"
#include "WebApplication/WebApplication.hpp"

#include "common.hpp"

class HttpServer {
  std::vector<ResponseMiddlewareHandler> _responseMiddlewareHandlers;
  std::vector<std::reference_wrapper<WebApplication>> _webApplications;

  Queue<std::shared_ptr<HttpResponse>> _responsesQueue;

  ServerConfiguration _configuration;

  std::unique_ptr<ILogger> _logger;

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

