#include "PiecesServer.hpp"

#include <fstream>
#include <iostream>

#define CLIENT_PORT 2816
#define BROWSER_PORT 2832
#define BUFFERSIZE 500

PiecesServer::PiecesServer(std::string legoSourceFileName,
      char type,
      bool ipv6):
    legoSourceFileName(legoSourceFileName),
    clientQueue(),
    browserQueue() {
  this->clientSocket = new Socket(type, ipv6);
  this->browserSocket = new Socket(type, ipv6);
}

int PiecesServer::readLegoSourceFile(std::string legoSourceFileName) {
  // if there is a new file name, replace the local one
  if (legoSourceFileName.size() != 0) {
    this->legoSourceFileName = legoSourceFileName;
  }

  std::string extension =
      this->legoSourceFileName.substr(this->legoSourceFileName.size() - 4, 4);

  if (extension != ".txt") {
    std::cerr <<
        "Provided file name does not have compatible .txt entension" <<
        std::endl;
    return -3;
  }

  std::fstream legoSourceFile;
  legoSourceFile.open(this->legoSourceFileName);

  std::string buffer = "";
  int count = 0;

  std::getline(legoSourceFile, buffer);

  if (buffer != "Lego source File :: group ESJOJO") {
    std::cerr <<
        "text file name is not compatible with \"Lego source File :: group ESJOJO\""
        << std::endl;
    return -1;
  }

  std::string legoTypeBuffer = "";

  while (std::getline(legoSourceFile, buffer)) {
    // first get the type of the lego
    if (count % 2 == 0) {
      legoTypeBuffer = buffer;
    // then once both lego and amount are available, add to map
    } else {
      legos[legoTypeBuffer] += atoi(buffer.c_str());
    }

    count++;
  }

  if (count % 2 != 0) {
    std::cerr << "Element mismatch error: file does not have equal amount of"
        << " lego types and amount" << std::endl; 
    return -2;
  }

  return EXIT_SUCCESS;
}

void PiecesServer::startServer() {
  Socket* client;

  this->clientSocket->Bind(CLIENT_PORT);
  this->clientSocket->Listen(5);
  this->clientSocket->SSLInitServer("esjojo.pem", "key.pem");

  this->browserSocket->Bind(BROWSER_PORT);
  this->browserSocket->Listen(5);
  this->browserSocket->SSLInitServer("esjojo.pem", "key.pem");

  std::cout << "listening" << std::endl;

  // make thread that listens browser requests
  this->browserListenThread =
      std::shared_ptr<std::thread>
      (new std::thread(listenBrowserConnections, this));

  // make thread that processes browser requests
  this->browserRequestHandler =
      std::shared_ptr<std::thread>
      (new std::thread(processBrowserRequests, this));

  // make thread that processes client requests
  this->clientRequestHandler =
      std::shared_ptr<std::thread>
      (new std::thread(processClientRequests, this));

  // make thread that listens to cin

  // look for connection requests on socket
  while (true) {
    std::cout << "Listening to client connections" << std::endl;

    client = this->clientSocket->Accept();
    
    std::cout << "Client connection accepted\n" << std::endl;

    if ((int)(size_t)client == -1) {
      this->clientQueue.push(nullptr);
      break;
    }

    // queue the requests
    this->clientQueue.push(client);
  }

  // join the threads
  this->browserListenThread->join();
  this->browserRequestHandler->join();
  this->clientRequestHandler->join();
}


