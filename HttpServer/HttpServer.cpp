//
// Created by josephvalverde on 12/11/23.
//

#include <filesystem>
#include "HttpServer.ixx"

HttpServer &HttpServer::getInstance() {
  static HttpServer instance;
  return instance;
}

void HttpServer::addConfiguration(const std::string& configurationJsonPath) {

}

[[noreturn]] void HttpServer::startServer() {
  this->setUpServer();

  this->tcpSocket->Listen(16);

  std::filesystem::path currentPath = __FILE__;
  currentPath = currentPath.parent_path();

  std::string sslCertPath = currentPath / this->configuration.sslCertFileName;
  std::string sslKey = currentPath / this->configuration.sslKeyFileName;

  this->tcpSocket->SSLInitServer(sslCertPath, sslKey);
  this->tcpListener->start();

  for (RequestMiddlewareHandler& handler : this->requestMiddlewareHandlers) {
    handler.start();
  }

  for (ApplicationMiddlewareHandler& handler : this->applicationMiddlewareHandlers) {
    handler.start();
  }

  for (ResponseMiddlewareHandler& handler : this->responseMiddlewareHandlers) {
    handler.start();
  }
}

[[noreturn]] void HttpServer::stopServer() {
  this->tcpListener->stop();
}

void HttpServer::setUpServer() {
  this->tcpSocket = std::make_shared<Socket>('s', false);
  this->tcpSocket->Bind(this->configuration.port);

  ListenerMessageBundle messages {
          "Listening for client connections (TCP)\n",
          "request received\n",
          "stopping server\n"
  };

  this->tcpListener = std::make_shared<TcpListener>(
          &(this->connectionsQueue)
          , this->tcpSocket
          , messages
          , nullptr
          , nullptr
          , this->configuration.port
  );

  for (int requestHandlerIndex = 0;
       requestHandlerIndex < this->configuration.requestHandlerAmount;
       ++requestHandlerIndex) {
    this->requestMiddlewareHandlers.emplace_back(
            &(this->connectionsQueue)
            , this->requestsQueue
            , nullptr
    );
  }

  for (int applicationHandlerIndex = 0;
       applicationHandlerIndex < this->configuration.applicationHandlerAmount;
       ++applicationHandlerIndex) {
    this->applicationMiddlewareHandlers.emplace_back(
            &(this->requestsQueue)
            , this->responsesQueue
            , nullptr
    );
  }

  for (int responseHandlerIndex = 0;
       responseHandlerIndex < this->configuration.responseHandlerAmount;
       ++responseHandlerIndex) {
    this->responseMiddlewareHandlers.emplace_back(
            &(this->responsesQueue)
            , nullptr
    );
  }
}


