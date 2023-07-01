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

  Queue<std::shared_ptr<std::vector<char>>> UDPSockets;
  Queue<std::shared_ptr<Socket>> TCPSockets;

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
        , nullptr
        , nullptr
        , PIECES_UDP_PORT
        , false
        , 'd'
        , udpListeningMessage
        , stopping
        , processing);

    this->handleUDP.push_back(new UDPHandler(&(this->UDPSockets)
        , nullptr));
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
    Socket broadcastSocket('d', false);
    sockaddr_in island;

    union message_t {
      int number;
      char values[4];
    };

    message_t code;

    code.number = LegoMessageCode::LEGO_PRESENT;

    // get the computer IP
    std::vector<char> broadcastMessage;

    broadcastMessage.resize(4);
    memcpy(broadcastMessage.data(), &code, 4);

    broadcastMessage.push_back(29);

    std::string buffer = getComputerIp();

    for (char character : buffer) {
      broadcastMessage.push_back(character);
    }
    
    broadcastMessage.push_back(':');

    buffer = std::to_string(INTERMEDIARY_TCP_PORT).data();

    for (char character : buffer) {
      broadcastMessage.push_back(character);
    }

    broadCastOnSamePC(island, broadcastMessage, broadcastSocket);

    // get the base for the broadcast IPs
    int broadcastIpId = 15;
    std::string broadcastIpBase = "172.16.123.";

    // for each vlan
    for (size_t vlan = 200; vlan < 207; vlan++) {
      // get the island broadcast ip
      std::string broadcastIp = broadcastIpBase + std::to_string(broadcastIpId);

      // set the ip and port for the message to be sent
      memset(&island, 0, sizeof(sockaddr_in));
      island.sin_family = AF_INET;
      island.sin_port = htons(INTERMEDIARY_UDP_PORT); // send to intermediary udp port
      inet_pton(AF_INET, broadcastIp.data(), &(island.sin_addr));

      std::cout << "Broadcasting on: " << broadcastIp << std::endl;

      // send the broadcast
      broadcastSocket.sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);

      // set the next broadcast ip
      broadcastIpId += 16;
    }

    std::cout << std::endl;
  }

  void broadCastOnSamePC(sockaddr_in& island,std::vector<char>& broadcastMessage, Socket& broadcastSocket) {
    memset(&island, 0, sizeof(sockaddr_in));
    island.sin_family = AF_INET;
    island.sin_port = htons(INTERMEDIARY_UDP_PORT);
    island.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Broadcasting on: same computer" << std::endl;

    // send the broadcast
    broadcastSocket.sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);
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

