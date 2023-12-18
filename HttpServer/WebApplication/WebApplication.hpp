//
// Created by josephvalverde on 12/17/23.
//

#ifndef LEGO_FIGURE_MAKER_WEBAPPLICATION_HPP
#define LEGO_FIGURE_MAKER_WEBAPPLICATION_HPP

#include <memory>

#include "JsonParsingPolicies/ServerConfigurationParsingPolicy.hpp"
#include "Socket/TcpSocket.hpp"
#include "Middleware/Handlers/RequestMiddlewareHandler.hpp"
#include "Logger/ILogger.hpp"
#include "Http/HttpMessages/HttpResponse.hpp"
#include "Middleware/Listeners/TcpListener.hpp"
#include "Middleware/Handlers/ApplicationMiddlewareHandler.hpp"

class WebApplication {
  std::shared_ptr<TcpSocket> _tcpSocket;
  std::optional<TcpListener> _tcpListener;

  std::vector<RequestMiddlewareHandler> _requestMiddlewareHandlers;
  std::vector<ApplicationMiddlewareHandler> _applicationMiddlewareHandlers;

  BlockingQueue<std::shared_ptr<TcpSocket>> _connectionsQueue;
  BlockingQueue<std::shared_ptr<HttpRequest>> _requestsQueue;
  std::optional<std::reference_wrapper<BlockingQueue<std::shared_ptr<HttpResponse>>>> _responsesQueue;

  ServerConfiguration _configuration {};

  std::unique_ptr<ILogger> _logger;

public:
  WebApplication() = default;

  NO_COPY(WebApplication)

  /**
   * @brief Sets the default configuration that the server will use if no configuration is provided.
   * @param fallBackConfiguration the default configuration
   */
  void setDefaultFallBackConfiguration(const ServerConfiguration& fallBackConfiguration);

  /**
   * Adds a configuration on how the server should behave.
   * @param configurationJsonPath the path of the file that has the configuration.
   */
  void addConfiguration(const std::string& configurationJsonPath);

  /**
   * Adds a queue that will be used to send responses to the client.
   * @param responsesQueue the queue that will be used to send responses to the client.
   */
  void addResponsesQueue(BlockingQueue<std::shared_ptr<HttpResponse>>& responsesQueue);

  /**
   * Initializes the application and starts its operation
   */
  void startApplication();

  /**
   * Waits for the application to finish its operation
   * @note This method is blocking
   * @remark This method should be called after startApplication()
   */
  void waitToFinish();

  /**
   * Stops the application
   */
  void stopApplication();

protected:
  inline void startResources();

};

#endif //LEGO_FIGURE_MAKER_WEBAPPLICATION_HPP
