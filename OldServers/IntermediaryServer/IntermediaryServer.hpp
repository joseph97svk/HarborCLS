#include <filesystem>

#include "../../HttpServer/ProtocolHeader.hpp"
#include "Listeners.hpp"
#include "Handlers/ClientHandler.hpp"
#include "Handlers/RequestHandler.hpp"
#include "Handlers/ResponseHandler.hpp"
#include "Handlers/UDPHandler.hpp"

#define CLIENTPORT 2020

class path;

import ListenerMessageBundle;

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

  std::shared_ptr<Socket> clientSocket;
  std::shared_ptr<Socket> connectionSocket;

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

    this->clientSocket = std::make_shared<Socket>('s', false);
    this->clientSocket->Bind(CLIENTPORT);

    this->connectionSocket = std::make_shared<Socket>('d', false);
    this->connectionSocket->Bind(INTERMEDIARY_UDP_PORT);

    ListenerMessageBundle clientMessages {
      "Listening for client connections (TCP)\n",
      "request received (TCP)\n",
      "stopping server (TCP)\n"
    };

    ListenerMessageBundle udpMessages {
      "Listening for pieces servers (UDP)\n",
      "request received (UDP)\n",
      "stopping server (UDP)\n"
    };

    this->listenClientConnections = new ClientListener(&(this->ClientRequests)
        , clientSocket
        , clientMessages
        , nullptr
        , nullptr
        , CLIENTPORT
      );
    this->listenPiecesServerUDP = new UDPListener(&(this->UDPRequests)
        , this->connectionSocket
        , udpMessages
        , nullptr
        , nullptr
        , INTERMEDIARY_UDP_PORT
        );

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
    this->clientSocket->setBufferDefault(1024);
    this->clientSocket->Listen(16);

    std::filesystem::path currentPath = __FILE__;
    currentPath = currentPath.parent_path();

    std::string sslCertPath = currentPath / this->SSLCert;
    std::string sslKey = currentPath / this->SSLKey;

    this->clientSocket->SSLInitServer(sslCertPath, sslKey);

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
  }

  void stopServer() {
    this->listenClientConnections->stop();
    this->listenPiecesServerUDP->stop();
  }

 private:
  void broadcastPresence() {
    std::vector<char> empty;
    std::cout << "Broadcasting for discover" << std::endl;
    this->broadcast(empty, LEGO_DISCOVER);
  }

  void broadcast(std::vector<char>& message, int code) {
    Socket broadcastSocket('d', false);
    sockaddr_in island;

    // get the computer IP
    std::vector<char> broadcastMessage;

    broadcastMessage.push_back(std::to_string(code)[0]);

    broadcastMessage.push_back(SEPARATOR);

    std::string buffer = getComputerIp();

    for (char character : buffer) {
      broadcastMessage.push_back(character);
    }

    broadcastMessage.push_back(':');

    buffer = std::to_string(INTERMEDIARY_UDP_PORT).data();

    for (char character : buffer) {
      broadcastMessage.push_back(character);
    }

    broadcastMessage.push_back(SEPARATOR);
    size_t startPosition = broadcastMessage.size();

    broadcastMessage.resize(broadcastMessage.size() + message.size());
    memcpy(&(broadcastMessage.data()[startPosition]), message.data(), message.size());

    // attempt on normal for computer
    broadCastOnSamePC(broadcastMessage, broadcastSocket);

    // get the base for the broadcast IPs
    int broadcastIpId = 15;
    std::string broadcastIpBase = "172.16.123.";

    broadcastSocket.setBroadcast(true);

    // for each vlan
    for (size_t vlan = 200; vlan < 207; vlan++) {
      broadcastIsland(island, broadcastMessage, broadcastSocket, broadcastIpId, broadcastIpBase);

      // set the next broadcast ip
      broadcastIpId += 16;
    }

    std::cout << std::endl;
  }

  void broadcastIsland(
      sockaddr_in& island,
      std::vector<char>& broadcastMessage,
      Socket& broadcastSocket,
      int broadcastIpId,
      std::string& broadcastIpBase
      ) {
    (void) island;
    // get the island broadcast ip
    std::string broadcastIp;
    broadcastIp.push_back('-');
    broadcastIp += broadcastIpBase + std::to_string(broadcastIpId);

    // set the ip and port for the message to be sent
    // memset(&island, 0, sizeof(sockaddr_in));
    // island.sin_family = AF_INET;
    // island.sin_port = htons(PIECES_UDP_PORT);
    // inet_pton(AF_INET, broadcastIp.data(), &(island.sin_addr));

    broadcastSocket(broadcastIp, PIECES_UDP_PORT) << broadcastMessage;

    std::cout << "Broadcasting on: " << broadcastIp.substr(1, broadcastIp.size()) << std::endl;

    // send the broadcast
    //broadcastSocket.sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);
  }

  void broadCastOnSamePC(std::vector<char>& broadcastMessage, Socket& broadcastSocket) {
    // memset(&island, 0, sizeof(sockaddr_in));
    // island.sin_family = AF_INET;
    // island.sin_port = htons(PIECES_UDP_PORT);
    // island.sin_addr.s_addr = INADDR_ANY;
    broadcastSocket("", PIECES_UDP_PORT) << broadcastMessage;

    std::cout << "Broadcasting on: same computer" << std::endl;

    // send the broadcast
    //broadcastSocket.sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);
  }
};