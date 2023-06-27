#include "Generics/ProtocolHeader.hpp"

#include "Listeners.hpp"
#include "Handlers.hpp"

class IntermediaryServer {
  Listener* listenClientConnections;
  Listener* listenPiecesServerUDP;
  Listener* listenPiecesServerTCP;

  std::vector<Handler<Socket*>*> handleClientConnections;
  std::vector<Handler<Socket*>*> handleUDP;
  std::vector<RequestHandler*> handleTCP;
  std::vector<ResponseHandler*> handleResponses;

  Queue<Socket*> ClientRequests;
  Queue<Socket*> UDPRequests;
  Queue<Request*> TCPRequests;
  Queue<std::shared_ptr<Response>> responseQueue;

  Socket* clientSocket;
  Socket* connectionSocket;

  RoutingMap* routingMap;

  IntermediaryServer() {
    this->clientSocket = new Socket('s', false);
    this->connectionSocket = new Socket('d', false);

    std::string stopping = "stopping server\n";
    std::string processing = "request received\b";
    this->listenClientConnections = new ClientListener(&(this->ClientRequests)
        , clientSocket
        , nullptr
        , nullptr
        , 80
        , true
        , 's'
        , processing
        , stopping);
    this->listenPiecesServerUDP = new UDPListener(&(this->UDPRequests)
        , nullptr // No need for a socket that will need to be destroyed and restarted for every connection
        , nullptr
        , nullptr
        , INTERMEDIARY_UDP_PORT
        , false
        , 'd'
        , processing
        , stopping);

    this->routingMap = new RoutingMap();
    
    this->handleClientConnections.push_back(new ClientHandler(&(this->ClientRequests)
        , nullptr));
    this->handleUDP.push_back(new UDPHandler(&(this->UDPRequests)
        , nullptr
        , this->routingMap));
    this->handleTCP.push_back(new RequestHandler(&(this->TCPRequests)
        , nullptr
        , this->routingMap
        , &(this->responseQueue)));
    this->handleResponses.push_back(new ResponseHandler(&(this->responseQueue)
        , nullptr));
  }

  void start() {
    broadcastPresence();

    // initiate all threads

    // start all listeners
    this->listenClientConnections->start();
    this->listenPiecesServerTCP->start();

    // start handlers
    for (Handler<Socket*>* handler : this->handleClientConnections) {
      handler->start();
    }

    for (Handler<Socket*>* handler : this->handleUDP) {
      handler->start();
    }

    for (RequestHandler* handler : this->handleTCP) {
      handler->start();
    }

    for (ResponseHandler* handler : this->handleResponses) {
      handler->start();
    }

    // join all threads

    // join listeners
    this->listenClientConnections->waitToFinish();
    this->listenPiecesServerUDP->waitToFinish();

    // join handlers
    for (Handler<Socket*>* handler : this->handleClientConnections) {
      handler->waitToFinish();
    }

    for (Handler<Socket*>* handler : this->handleUDP) {
      handler->waitToFinish();
    }

    for (RequestHandler* handler : this->handleTCP) {
      handler->waitToFinish();
    }

    for (ResponseHandler* handler : this->handleResponses) {
      handler->waitToFinish();
    }
  }

  void broadcastPresence() {

  }

  void stopServer() {
    this->listenClientConnections->stop();
    this->listenPiecesServerTCP->stop();
    this->listenPiecesServerTCP->stop();
  }
};