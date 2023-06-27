#include "Handlers.hpp"
#include "Listeners.hpp"
#include "Generics/ProtocolHeader.hpp"
#include <vector>

class PiecesServer {
  Listener* listenUDP;
  Listener* listenTCP;

  std::vector<Handler*> handleUDP;
  std::vector<Handler*> handleTCP;

  Queue<Socket*> UDPSockets;
  Queue<Socket*> TCPSockets;

  Socket* connectionSocket;
  Socket* communicationsSocket;

  PiecesServer() {
    std::string stopping = "stopping server\n";
    std::string processing = "request received\b";

    this->listenTCP = new TCPListener(&(this->TCPSockets)
        , communicationsSocket
        , nullptr
        , nullptr
        , PIECES_TCP_PORT
        , true
        , 's'
        , stopping
        , processing);
    this->listenUDP = new UDPListener(&(this->UDPSockets)
        , connectionSocket
        , nullptr
        , nullptr
        , PIECES_UDP_PORT
        , false
        , 'd'
        , stopping
        , processing);

    this->handleUDP.push_back(new UDPHandler(&(this->UDPSockets)
        , nullptr));
    this->handleTCP.push_back(new TCPHandler(&(this->TCPSockets)
        , nullptr));

    this->connectionSocket = new Socket('d', false);
    this->communicationsSocket = new Socket('s', false);
  }

  void start() {
    broadcastPresence();

    // initiate all threads

    // start listeners
    this->listenTCP->start();
    this->listenUDP->start();

    // start handlers
    for (Handler* handler : this->handleUDP) {
      handler->start();
    }

    for (Handler* handler : this->handleTCP) {
      handler->start();
    }

    // join all threads

    // join listeners
    this->listenTCP->waitToFinish();
    this->listenUDP->waitToFinish();

    // join handlers
    for (Handler* handler : this->handleUDP) {
      handler->waitToFinish();
    }

    for (Handler* handler : this->handleTCP) {
      handler->waitToFinish();
    }
  }

  void broadcastPresence() {

  }

  void stopServer() {
    this->listenTCP->stop();
    this->listenUDP->stop();
  }
};