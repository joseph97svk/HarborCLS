#include "PiecesServer.hpp"

#include <fstream>
#include <iostream>

/**
 * Constructor for the PiecesServer class.
 * @param legoSourceFileName The name of the Lego source file.
 * @param type The type of socket to be created (e.g., TCP or UDP).
 * @param ipv6 A boolean value indicating whether to use IPv6 or not.
 */
PiecesServer::PiecesServer(std::string legoSourceFileName,
      char type,
      bool ipv6):
    legoSourceFileName(legoSourceFileName),
    clientQueue(),
    browserQueue() {
  this->clientSocket = new Socket(type, ipv6);
  this->browserSocket = new Socket(type, ipv6);
}

/**
 * Stop the PiecesServer by closing the server sockets and joining the threads.
 */
void PiecesServer::stop() {
  // Stop listening for incoming client connection requests
  this->closing = true;

  // Create a temporary closing client socket for signaling
  Socket closingClient('s', false);

  // Connect to a specified IP address and port to signal the server closure
  closingClient.Connect("ip aqui", BROWSER_PORT);

  std::cout << "joining threads..." << std::endl;

  // join the threads
  this->browserListenThread->join();
  this->browserRequestHandler->join();
  std::cout << "browser liner and handler thread joined" << std::endl;

  // Close the browser socket and deallocate the memory
  this->browserSocket->Close();
  delete this->browserSocket;
}

/**
 * Get the singleton instance of the PiecesServer.
 * @return A reference to the PiecesServer singleton instance.
 */
PiecesServer& PiecesServer::getInstance() {
  static PiecesServer piecesServer("legoSource.txt", 's', false);
  return piecesServer;
  }

/**
 * Read the Lego source file and populate the lego inventory map.
 * @param legoSourceFileName The name of the Lego source file to be read.
 * @return An integer representing the result of the file reading operation.
 *         0: Successful file reading.
 *        -1: Incompatible text file name.
 *        -2: Element mismatch error.
 *        -3: Incompatible file extension.
 */
int PiecesServer::readLegoSourceFile(std::string legoSourceFileName) {
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
  std::fstream legoSourceFile;
  legoSourceFile.open(this->legoSourceFileName);

  std::string buffer = "";
  int count = 0;

  // Read the first line of the file
  std::getline(legoSourceFile, buffer);

  if (buffer != "Lego source File :: group ESJOJO") {
    std::cerr <<
        "text file name is not compatible with \"Lego source File :: group ESJOJO\""
        << std::endl;
    return -1;
  }

  std::string legoTypeBuffer = "";

  // Read the remaining lines of the file
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

  return EXIT_SUCCESS; // Return success status
}

/**
 * Start the server by initializing sockets, creating threads, and listening for client connections.
 */
void PiecesServer::startServer() {
  Socket* client;

  // Bind the client socket to the CLIENT_PORT and initialize SSL server
  this->clientSocket->Bind(CLIENT_PORT);
  this->clientSocket->Listen(5);
  this->clientSocket->SSLInitServer("esjojo.pem", "key.pem");

  // Bind the browser socket to the BROWSER_PORT
  this->browserSocket->Bind(BROWSER_PORT);
  this->browserSocket->Listen(5);

  std::cout << "listening" << std::endl;

  // Create a thread for listening to browser connections
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
    
    if ((int)(size_t)client == -1 || client == nullptr || this->closing) {
      this->clientQueue.push(nullptr);
      break;
    }
    // queue the requests
    this->clientQueue.push(client);
  }

  if (!this->closing) {
    
  }

  // Join the client request handler thread
  this->clientRequestHandler->join();
  std::cout << "Client threads joined" << std::endl;

  // Close the client socket and deallocate the memory
  this->clientSocket->Close();
  delete this->clientSocket;
}


