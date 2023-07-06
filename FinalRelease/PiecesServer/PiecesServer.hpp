#include "Handlers.hpp"
#include "Listeners.hpp"
#include "Generics/ProtocolHeader.hpp"
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <map>
#include <fstream>
#include <iostream>

class PiecesServer {

 private:
  UDPListener* listenUDP;
  TCPListener* listenTCP;

  std::vector<UDPHandler*> handleUDP;
  std::vector<TCPHandler*> handleTCP;

  Queue<std::shared_ptr<std::vector<char>>> UDPSockets;
  Queue<std::shared_ptr<Socket>> TCPSockets;

  Socket* connectionSocket;
  Socket* communicationsSocket;

  std::string legoSourceFileName;
  LegoMap myFigures;

 public:
  PiecesServer(std::string legoSourceFileName = "") {
    std::string stopping = "stopping server\n";
    std::string processing = "request received\b";

    std::string clientListeningMessage = "Listening for client connections (TCP)\n";
    std::string udpListeningMessage = "Listening for pieces servers (UDP)\n";
    this->legoSourceFileName = legoSourceFileName;

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
        , nullptr, &this->myFigures));
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

    // get the computer IP
    std::vector<char> broadcastMessage;

    broadcastMessage.push_back(std::to_string(int(LegoMessageCode::LEGO_PRESENT))[0]);

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

    broadcastMessage.push_back(29);

    // #define test

    # ifdef test

    std::string figure = "Chicki";

    // add figure
    for (char character : figure) {
      broadcastMessage.push_back(character);
    }

    #else
  
    // for all figures
    for (auto& figure : this->myFigures) {
      // add the figure name
      for (char character : figure.first) {
        // add it to the message
        broadcastMessage.push_back(character);
      }

      // add separator
      broadcastMessage.push_back(29);
    }

    #endif
    std::string msg;
    msg.resize(broadcastMessage.size());
    memcpy(&(msg.data()[0]), &(broadcastMessage.data()[0]), broadcastMessage.size());
    
    std::cout << msg << std::endl;
    // attempt on normal for computer
    broadCastOnSamePC(island, broadcastMessage, broadcastSocket);

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

  // PiecesServer& getInstance() {
  //   static PiecesServer piecesServer("legoSource.txt");
  //   return piecesServer;
  // }
 public:
  int readLegoSourceFile(std::string legoSourceFileName) {
    // if there is a new file name, replace the local one
    if (legoSourceFileName.size() != 0) {
      this->legoSourceFileName = legoSourceFileName;
    }

    // Extract the file extension
    std::string extension =
        this->legoSourceFileName.substr(this->legoSourceFileName.size() - 4, 4);

    // Check if the file has a compatible .txt extension
    if (extension != ".txt") {
      std::cerr <<
          "Provided file name does not have compatible .txt entension" <<
          std::endl;
      return -3;
    }

    // Open the Lego source file
    std::fstream fileLego;
    fileLego.open("legoFile.txt");
    std::string buffer = "";

    if (!fileLego.is_open()) {
      std::cout << "No se pudo abrir el archivo." << std::endl;
      return -4;
    } 

    // Read the first line of the file
    std::getline(fileLego, buffer);
    std::cout << buffer << std::endl;
    if (buffer != "Lego source File :: group ESJOJO") {
      std::cerr <<
          "text file name is not compatible with \"Lego source File :: group ESJOJO\""
          << std::endl;
      return -1;
    }

    std::string figureName;
    std::string figureImage;
    std::string pieceName;
    std::string pieceImage;
    size_t pieceAmount;

    while (std::getline(fileLego, figureName)) {
      std::getline(fileLego, figureImage);
      this->myFigures[figureName].first = figureImage;

      while (std::getline(fileLego, pieceName)) {
        if (pieceName == "*") {
          break;
        }

        std::getline(fileLego, pieceImage);
        fileLego >> pieceAmount;
        fileLego.ignore();

        myFigures[figureName].second.push_back(Lego(pieceImage, pieceName, pieceAmount));
      }
    }
    // Acceder y mostrar los datos del std::map
    for (const auto& figure : myFigures) {
        std::cout << "Figure: " << figure.first << std::endl;
        std::cout << "Image: " << figure.second.first << std::endl;

        for (const auto& piece : figure.second.second) {
            std::cout << "Piece Name: " << piece.description << std::endl;
            std::cout << "Piece Image: " << piece.imageFigure << std::endl;
            std::cout << "Piece Amount: " << piece.amount << std::endl;
        }

        std::cout << std::endl;
    }
    return EXIT_SUCCESS; // Return success status
  }

 public:
  void stopServer() {
    this->listenTCP->stop();
    this->listenUDP->stop();
  }
};

