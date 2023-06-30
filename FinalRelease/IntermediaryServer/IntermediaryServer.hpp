#include "Generics/ProtocolHeader.hpp"

#include "Listeners.hpp"
#include "Handlers.hpp"

#define CLIENTPORT 2019

class IntermediaryServer {
  ClientListener* listenClientConnections;
  UDPListener* listenPiecesServerUDP;

  std::vector<Handler<Socket*>*> handleClientConnections;
  std::vector<UDPHandler*> handleUDP;
  std::vector<RequestHandler*> handleRequest;
  std::vector<ResponseHandler*> handleResponses;

  Queue<Socket*> ClientRequests;
  Queue<std::shared_ptr<std::pair<std::string, int>>> UDPRequests;
  Queue<Request*> RequestQueue;
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
    for (Handler<Socket*>* handler : this->handleClientConnections) {
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
    for (Handler<Socket*>* handler : this->handleClientConnections) {
      handler->waitToFinish();
    }

    for (UDPHandler* handler : this->handleUDP) {
      handler->waitToFinish();
    }

    for (RequestHandler* handler : this->handleRequest) {
      handler->waitToFinish();
    }

    for (ResponseHandler* handler : this->handleResponses) {
      handler->waitToFinish();
    }

    this->clientSocket->Close();
    //this->connectionSocket->Close();

    delete this->clientSocket;
    delete this->connectionSocket;
  }

  void stopServer() {
    this->listenClientConnections->stop();
    this->listenPiecesServerUDP->stop();
  }

 private:
  void broadcastPresence() {
    std::vector<sockaddr_in> islands(7);

    // get the computer IP
    std::string broadcastMessage = getComputerIp();

    // get the base for the broadcast IPs
    int broadcastIpId = 15;
    std::string broadcastIpBase = std::to_string(INTERMEDIARY_TCP_PORT) + ":172.16.123.";

    // for each island
    for (sockaddr_in island : islands) {
      // get the island broadcast ip
      std::string broadcastIp = broadcastIpBase + std::to_string(broadcastIpId);

      // set the ip and port for the message to be sent
      memset(&island, 0, sizeof(sockaddr_in));
      islands[0].sin_port = htons(PIECES_UDP_PORT);
      inet_pton(AF_INET, broadcastIp.data(), &(islands[0].sin_addr));

      std::cout << "Broadcasting on: " << broadcastIp << std::endl;

      // send the broadcast
      this->connectionSocket->sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);

      // set the next broadcast ip
      broadcastIpId += 16;
    }

    std::cout << std::endl;
  }
};