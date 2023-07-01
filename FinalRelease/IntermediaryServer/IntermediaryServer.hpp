#include "Generics/ProtocolHeader.hpp"

#include "Listeners.hpp"
#include "Handlers.hpp"

#define CLIENTPORT 2020

class IntermediaryServer {
  ClientListener* listenClientConnections;
  UDPListener* listenPiecesServerUDP;

  std::vector<ClientHandler*> handleClientConnections;
  std::vector<UDPHandler*> handleUDP;
  std::vector<RequestHandler*> handleRequest;
  std::vector<ResponseHandler*> handleResponses;

  Queue<std::shared_ptr<Socket>> ClientRequests;
  Queue<std::shared_ptr<std::vector<char>>> UDPRequests;
  Queue<std::shared_ptr<Request>> RequestQueue;
  Queue<std::shared_ptr<Response>> responseQueue;

  Socket* clientSocket;
  Socket* connectionSocket;

  std::string SSLCert;
  std::string SSLKey;

  RoutingMap* routingMap;

 public:
  static IntermediaryServer& getInstance(
      std::string& SSLCert,
      std::string& SSLKey
      ) {
    static IntermediaryServer server(SSLCert, SSLKey);
    return server;
  }

 private:
  IntermediaryServer(
      std::string& SSLCert,
      std::string& SSLKey)
    : SSLCert(SSLCert)
    , SSLKey(SSLKey) {

    this->clientSocket = new Socket('s', false);
    this->clientSocket->Bind(CLIENTPORT);

    this->connectionSocket = new Socket('d', false);
    this->connectionSocket->Bind(INTERMEDIARY_UDP_PORT);

    std::string stoppingUDP = "stopping server (UDP)\n";
    std::string processingUDP = "request received (UDP)\n";
    std::string stoppingTCP = "stopping server (TCP)\n";
    std::string processingTCP = "request received (TCP)\n";
    std::string clientListeningMessage = "Listening for client connections (TCP)\n";
    std::string udpListeningMessage = "Listening for pieces servers (UDP)\n";
    this->listenClientConnections = new ClientListener(&(this->ClientRequests)
        , clientSocket
        , nullptr
        , nullptr
        , CLIENTPORT
        , true
        , 's'
        , clientListeningMessage
        , processingTCP
        , stoppingTCP);
    this->listenPiecesServerUDP = new UDPListener(&(this->UDPRequests)
        , this->connectionSocket
        , nullptr
        , nullptr
        , INTERMEDIARY_UDP_PORT
        , false
        , 'd'
        , udpListeningMessage
        , processingUDP
        , stoppingUDP);

    this->routingMap = new RoutingMap();

    this->handleClientConnections.push_back(new ClientHandler(&(this->ClientRequests)
        , &(this->RequestQueue)
        , nullptr));
    this->handleUDP.push_back(new UDPHandler(&(this->UDPRequests)
        , nullptr
        , this->routingMap));
    this->handleRequest.push_back(new RequestHandler(&(this->RequestQueue)
        , nullptr
        , this->routingMap
        , &(this->responseQueue)));
    this->handleResponses.push_back(new ResponseHandler(&(this->responseQueue)
        , nullptr));
  }

 public:
  void start() {
    this->clientSocket->Listen(16);
    this->clientSocket->SSLInitServer(this->SSLCert.data(), this->SSLKey.data());

    this->listenClientConnections->setAsUDP();

    // initiate all threads

    // start all listeners
    broadcastPresence();
    this->listenPiecesServerUDP->start();
    this->listenClientConnections->start();

    // start handlers
    for (ClientHandler* handler : this->handleClientConnections) {
      handler->start();
    }

    for (UDPHandler* handler : this->handleUDP) {
      handler->start();
    }

    for (RequestHandler* handler : this->handleRequest) {
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
    for (ClientHandler* handler : this->handleClientConnections) {
      handler->waitToFinish();
      delete handler;
    }

    for (UDPHandler* handler : this->handleUDP) {
      handler->waitToFinish();
      delete handler;
    }

    for (RequestHandler* handler : this->handleRequest) {
      handler->waitToFinish();
      delete handler;
    }

    for (ResponseHandler* handler : this->handleResponses) {
      handler->waitToFinish();
      delete handler;
    }

    delete this->listenClientConnections;
    delete this->listenPiecesServerUDP;

    this->clientSocket->Close();
    this->connectionSocket->Close();

    delete this->clientSocket;
    delete this->connectionSocket;
  }

  void stopServer() {
    this->listenClientConnections->stop();
    this->listenPiecesServerUDP->stop();
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

    code.number = LegoMessageCode::LEGO_DISCOVER;

    // get the computer IP
    std::vector<char> broadcastMessage;

    for (size_t byte = 0; byte < sizeof(LegoMessageCode); byte++) {
      broadcastMessage.push_back(code.values[byte]);
    }

    broadcastMessage.push_back(29);

    std::string buffer = getComputerIp().data();

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
      island.sin_port = htons(PIECES_UDP_PORT);
      inet_pton(AF_INET, broadcastIp.data(), &(island.sin_addr));

      std::cout << "Broadcasting on: " << broadcastIp << std::endl;

      // send the broadcast
      broadcastSocket.sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);

      // set the next broadcast ip
      broadcastIpId += 16;
    }

    std::cout << std::endl;
  }

  void broadCastOnSamePC(sockaddr_in& island, std::vector<char>& broadcastMessage, Socket& broadcastSocket) {
    memset(&island, 0, sizeof(sockaddr_in));
    island.sin_family = AF_INET;
    island.sin_port = htons(PIECES_UDP_PORT);
    island.sin_addr.s_addr = INADDR_ANY;

    std::cout << "Broadcasting on: same computer" << std::endl;

    // send the broadcast
    broadcastSocket.sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);
  }
};