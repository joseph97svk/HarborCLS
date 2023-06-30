#include "Generics/ProtocolHeader.hpp"

#include "Listeners.hpp"
#include "Handlers.hpp"

#define CLIENTPORT 2005

class IntermediaryServer {
  Listener* listenClientConnections;
  Listener* listenPiecesServerUDP;

  std::vector<Handler<Socket*>*> handleClientConnections;
  std::vector<Handler<Socket*>*> handleUDP;
  std::vector<RequestHandler*> handleRequest;
  std::vector<ResponseHandler*> handleResponses;

  Queue<Socket*> ClientRequests;
  Queue<Socket*> UDPRequests;
  Queue<Request*> RequestQueue;
  Queue<std::shared_ptr<Response>> responseQueue;

  Socket* clientSocket;
  Socket* connectionSocket;

  RoutingMap* routingMap;

 public:
  IntermediaryServer() {
    this->clientSocket = new Socket('s', false);
    this->clientSocket->Bind(CLIENTPORT);

    //this->connectionSocket = new Socket('d', false);

    std::string stopping = "stopping server\n";
    std::string processing = "request received\b";
    this->listenClientConnections = new ClientListener(&(this->ClientRequests)
        , clientSocket
        , nullptr
        , nullptr
        , CLIENTPORT
        , true
        , 's'
        , processing
        , stopping);
    // this->listenPiecesServerUDP = new UDPListener(&(this->UDPRequests)
    //     , nullptr // No need for a socket that will need to be destroyed and restarted for every connection
    //     , nullptr
    //     , nullptr
    //     , INTERMEDIARY_UDP_PORT
    //     , false
    //     , 'd'
    //     , processing
    //     , stopping);

    this->routingMap = new RoutingMap();

    this->handleClientConnections.push_back(new ClientHandler(&(this->ClientRequests)
        , &(this->RequestQueue)
        , nullptr));
    // this->handleUDP.push_back(new UDPHandler(&(this->UDPRequests)
    //     , nullptr
    //     , this->routingMap));
    this->handleRequest.push_back(new RequestHandler(&(this->RequestQueue)
        , nullptr
        , this->routingMap
        , &(this->responseQueue)));
    this->handleResponses.push_back(new ResponseHandler(&(this->responseQueue)
        , nullptr));
  }

  void start() {
    broadcastPresence();

    this->clientSocket->Listen(15);

    // initiate all threads

    // start all listeners
    this->listenClientConnections->start();
    // this->listenPiecesServerUDP->start();

    // start handlers
    for (Handler<Socket*>* handler : this->handleClientConnections) {
      handler->start();
    }

    // for (Handler<Socket*>* handler : this->handleUDP) {
    //   handler->start();
    // }

    for (RequestHandler* handler : this->handleRequest) {
      handler->start();
    }

    for (ResponseHandler* handler : this->handleResponses) {
      handler->start();
    }

    // join all threads

    // join listeners
    this->listenClientConnections->waitToFinish();
    // this->listenPiecesServerUDP->waitToFinish();

    // join handlers
    for (Handler<Socket*>* handler : this->handleClientConnections) {
      handler->waitToFinish();
    }

    // for (Handler<Socket*>* handler : this->handleUDP) {
    //   handler->waitToFinish();
    // }

    // for (RequestHandler* handler : this->handleRequest) {
    //   handler->waitToFinish();
    // }

    // for (ResponseHandler* handler : this->handleResponses) {
    //   handler->waitToFinish();
    // }
  }

 private:
  void broadcastPresence() {
    
  }

  void stopServer() {
    this->listenClientConnections->stop();
    this->listenPiecesServerUDP->stop();
  }
};