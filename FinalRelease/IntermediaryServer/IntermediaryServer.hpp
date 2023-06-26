#include "Generics/Handler.hpp"
#include "Generics/Listener.hpp"
#include "Generics/ProtocolHeader.hpp"

#include "Listeners.hpp"
#include "Handlers.hpp"

class IntermediaryServer {
  Listener* listenClientConnections;
  Listener* listenPiecesServerUDP;
  Listener* listenPiecesServerTCP;

  std::vector<Handler*> handleClientConnections;
  std::vector<Handler*> handleUDP;
  std::vector<Handler*> handleTCP;

  Queue<Socket*> ClientRequests;
  Queue<Socket*> UDPRequests;
  Queue<Socket*> TCPRequests;

  Socket* clientSocket;
  Socket* connectionSocket;
  Socket* communicationsSocket;

  IntermediaryServer() {
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
    this->listenPiecesServerTCP = new TCPListener(&(this->TCPRequests)
        , connectionSocket
        , nullptr
        , nullptr
        , INTERMEDIARY_TCP_PORT
        , true
        , 's'
        , processing
        , stopping);
    this->listenPiecesServerUDP = new UDPListener(&(this->UDPRequests)
        , communicationsSocket
        , nullptr
        , nullptr
        , INTERMEDIARY_UDP_PORT
        , false
        , 'd'
        , processing
        , stopping);

    this->handleClientConnections.push_back(new ClientHandler(&(this->ClientRequests)
        , nullptr));
    this->handleUDP.push_back(new UDPHandler(&(this->UDPRequests)
        , nullptr));
    this->handleTCP.push_back(new TCPHandler(&(this->TCPRequests)
        , nullptr));

    this->clientSocket = new Socket('s', false);
    this->connectionSocket = new Socket('d', false);
    this->communicationsSocket = new Socket('s', false);
  }

  void start() {
    // initiate all threads

    // start all listeners
    this->listenClientConnections->start();
    this->listenPiecesServerTCP->start();
    this->listenPiecesServerTCP->start();

    // start handlers
    for (Handler* handler : this->handleClientConnections) {
      handler->start();
    }

    for (Handler* handler : this->handleUDP) {
      handler->start();
    }

    for (Handler* handler : this->handleTCP) {
      handler->start();
    }

    // join all threads

    // start listeners
    this->listenClientConnections->waitToFinish();
    this->listenPiecesServerTCP->waitToFinish();
    this->listenPiecesServerTCP->waitToFinish();

    // start handlers
    for (Handler* handler : this->handleClientConnections) {
      handler->waitToFinish();
    }

    for (Handler* handler : this->handleUDP) {
      handler->waitToFinish();
    }

    for (Handler* handler : this->handleTCP) {
      handler->waitToFinish();
    }
  }

  void stopServer() {
    this->listenClientConnections->stop();
    this->listenPiecesServerTCP->stop();
    this->listenPiecesServerTCP->stop();
  }
};