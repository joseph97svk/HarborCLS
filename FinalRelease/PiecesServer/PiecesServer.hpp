#include "Handlers.hpp"
#include "Listeners.hpp"
#include "Generics/ProtocolHeader.hpp"
#include <vector>

class PiecesServer {
  UDPListener* listenUDP;
  TCPListener* listenTCP;

  std::vector<UDPHandler*> handleUDP;
  std::vector<TCPHandler*> handleTCP;

  Queue<std::pair<std::string, int>> UDPSockets;
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
        , {"", -1}
        , {"", -1}
        , PIECES_UDP_PORT
        , false
        , 'd'
        , stopping
        , processing);

    this->handleUDP.push_back(new UDPHandler(&(this->UDPSockets)
        , {"", -1}));
    this->handleTCP.push_back(new TCPHandler(&(this->TCPSockets)
        , nullptr));

    this->connectionSocket = new Socket('d', false);
    this->communicationsSocket = new Socket('s', false);
  }

  void start() {
    broadcastPresence();

    // bind server to port to listen
    this->connectionSocket->Bind(PIECES_UDP_PORT);

    this->communicationsSocket->Bind(PIECES_TCP_PORT);
    this->communicationsSocket->Listen(16);
    this->communicationsSocket->SSLInitServer("ci0123.pem", "key.pem");

    // initiate all threads

    // start listeners
    this->listenUDP->start();
    this->listenTCP->start();

    // start handlers
    for (UDPHandler* handler : this->handleUDP) {
      handler->start();
    }

    for (TCPHandler* handler : this->handleTCP) {
      handler->start();
    }

    // join all threads

    // join listeners
    this->listenTCP->waitToFinish();
    this->listenUDP->waitToFinish();

    // join handlers
    for (UDPHandler* handler : this->handleUDP) {
      handler->waitToFinish();
    }

    for (TCPHandler* handler : this->handleTCP) {
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