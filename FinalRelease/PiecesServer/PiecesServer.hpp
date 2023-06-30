#include "Handlers.hpp"
#include "Listeners.hpp"
#include "Generics/ProtocolHeader.hpp"
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

class PiecesServer {
  UDPListener* listenUDP;
  TCPListener* listenTCP;

  std::vector<UDPHandler*> handleUDP;
  std::vector<TCPHandler*> handleTCP;

  Queue<std::pair<std::string, int>> UDPSockets;
  Queue<Socket*> TCPSockets;

  Socket* connectionSocket;
  Socket* communicationsSocket;

 public:
  PiecesServer() {
    std::string stopping = "stopping server\n";
    std::string processing = "request received\b";

    std::string clientListeningMessage = "Listening for client connections (TCP)\n";
    std::string udpListeningMessage = "Listening for pieces servers (UDP)\n";

    this->connectionSocket = new Socket('d', false);
    this->communicationsSocket = new Socket('s', false);

    this->connectionSocket->Bind(PIECES_UDP_PORT);
    this->communicationsSocket->Bind(PIECES_TCP_PORT);

    this->listenTCP = new TCPListener(&(this->TCPSockets)
        , this->communicationsSocket
        , nullptr
        , nullptr
        , PIECES_TCP_PORT
        , true
        , 's'
        , clientListeningMessage
        , stopping
        , processing);

    this->listenUDP = new UDPListener(&(this->UDPSockets)
        , this->connectionSocket
        , {"", -1}
        , {"", -1}
        , PIECES_UDP_PORT
        , false
        , 'd'
        , udpListeningMessage
        , stopping
        , processing);

    this->handleUDP.push_back(new UDPHandler(&(this->UDPSockets)
        , {"", -1}));
    this->handleTCP.push_back(new TCPHandler(&(this->TCPSockets)
        , nullptr));
  }

  void start() {
    // bind server to port to listen
  
    this->communicationsSocket->Listen(16);
    this->communicationsSocket->SSLInitServer("esjojo.pem", "key.pem");

    // initiate all threads
    // start listeners
    broadcastPresence();
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

 private:
  void broadcastPresence() {
    std::vector<sockaddr_in> islands(7);

    // get the computer IP
    std::string broadcastMessage = getComputerIp();

    // get the base for the broadcast IPs
    int broadcastIpId = 15;
    std::string broadcastIpBase = std::to_string(PIECES_TCP_PORT) + ":172.16.123.";

    // for each island
    for (sockaddr_in island : islands) {
      // get the island broadcast ip
      std::string broadcastIp = broadcastIpBase + std::to_string(broadcastIpId);

      // set the ip and port for the message to be sent
      memset(&island, 0, sizeof(sockaddr_in));
      islands[0].sin_port = htons(INTERMEDIARY_UDP_PORT);
      inet_pton(AF_INET, broadcastIp.data(), &(islands[0].sin_addr));

      std::cout << "Broadcasting on: " << broadcastIp << std::endl;

      // send the broadcast
      this->connectionSocket->sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);

      // set the next broadcast ip
      broadcastIpId += 16;
    }

    std::cout << std::endl;
  }

  std::string getComputerIp () {
    std::string host;
    host.resize(256);

    gethostname(host.data(), sizeof(host));

    struct hostent* hostentry = gethostbyname(host.data());

    host.clear();
    host = inet_ntoa(*((struct in_addr*) hostentry->h_addr_list[0]));

    return host;
  }

 public:
  void stopServer() {
    this->listenTCP->stop();
    this->listenUDP->stop();
  }
};

