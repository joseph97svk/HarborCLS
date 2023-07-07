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

#include "common.hpp"

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
    std::string processing = "request received\n";

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
        , processing
        , stopping);

    this->listenUDP = new UDPListener(&(this->UDPSockets)
        , this->connectionSocket
        , nullptr
        , nullptr
        , PIECES_UDP_PORT
        , false
        , 'd'
        , udpListeningMessage
        , processing
        , stopping);

    this->handleUDP.push_back(
        new UDPHandler(&(this->UDPSockets)
        , nullptr
        , &(this->myFigures)));
    this->handleTCP.push_back(
        new TCPHandler(&(this->TCPSockets)
        , nullptr
        , &this->myFigures));


    this->listenUDP->setAsUDP();
  }
  /**
 * Get the singleton instance of the PiecesServer.
 * @return A reference to the PiecesServer singleton instance.
 */
  static PiecesServer& getInstance() {
    static PiecesServer piecesServer("legoFile.txt");
    return piecesServer;
  }

  void start() {
    // bind server to port to listen
    this->communicationsSocket->setBufferDefault(1024);
    this->communicationsSocket->Listen(16);
    this->communicationsSocket->SSLInitServer("ci0123.pem", "ci0123.pem");

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
  void release() {
    std::vector<char> empty;
    std::cout << "Sending realease" << std::endl;
    broadcast(empty, LEGO_RELEASE);
  }

  void broadcastPresence() {
    std::vector<char> broadcastMessage;

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
      broadcastMessage.push_back(SEPARATOR);
    }

    #endif
    broadcast(broadcastMessage, LEGO_PRESENT);
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

    //Acceder y mostrar los datos del std::map
    for (const auto& figure : myFigures) {
        std::cout << "Figure: " << figure.first << std::endl;
        // std::cout << "Image: " << figure.second.first << std::endl;

        // for (const auto& piece : figure.second.second) {
        //     std::cout << "Piece Name: " << piece.description << std::endl;
        //     std::cout << "Piece Image: " << piece.imageFigure << std::endl;
        //     std::cout << "Piece Amount: " << piece.amount << std::endl;
        // }

        std::cout << std::endl;
    }
    return EXIT_SUCCESS; // Return success status
  }

 public:
  void stopServer() {
    // tell intermediary servers to remove self from routing maps
    this->release();
    this->listenTCP->stop();
    this->listenUDP->stop();
  }
};

