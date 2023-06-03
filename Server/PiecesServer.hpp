#ifndef PIECESSERVER_HPP
#define PIECESSERVER_HPP

#include <string>
#include <map>
#include <thread>
#include <memory>
#include <regex>
#include <iostream>
#include <utility>

#include "Socket.hpp"
#include "Queue.hpp"

#define CLIENT_PORT 2816
#define BROWSER_PORT 2832
#define BUFFERSIZE 500

class PiecesServer {

  Socket* clientSocket;
  Socket* browserSocket;

  std::string legoSourceFileName;

  std::map<std::string, size_t> legos;

  std::shared_ptr<std::thread> listeningThread;
  std::shared_ptr<std::thread> browserListenThread;
  std::shared_ptr<std::thread> clientRequestHandler;
  std::shared_ptr<std::thread> browserRequestHandler;

  Queue<Socket*> clientQueue;
  Queue<Socket*> browserQueue;
  bool closing = false;

 public:
  PiecesServer(std::string legoSourceFileName = "",
    char type = 's',
    bool ipv6 = false);
  
  static PiecesServer& getInstance();


  int readLegoSourceFile(std::string legoSourceFileName = "");

  void startServer();
  void stop();

 private:

  /**
   * Process browser requests by popping clients from the browser queue and serving their requests.
   * @param server A pointer to the PiecesServer instance.
   */
  static void processBrowserRequests(PiecesServer* server) {
    Socket* client = nullptr;

    // Continuously process browser requests until signaled to stop
    while (true) {
      client = server->browserQueue.pop(); // Pop a client from the browser queue

      // Check if there is an error in popping the client or if the server is closing
      if ((int)(size_t)client == -1 || client == nullptr) {
        break;
      }

      // Process the browser request for the client using the legos map
      processBrowserRequest(client, server->legos);

      std::cout << "Browser request served" << std::endl;
    }

    // Once browser requests are done, close client requests
    // Since the main thread may be stolen by a signal, it must first finish signal handling
    // before the main thread running on client listening can begin ending
    server->clientSocket->Close();
  }

  /**
   * Process client requests by popping clients from the client queue and serving their requests.
   * @param server A pointer to the PiecesServer instance.
   */
  static void processClientRequests(PiecesServer* server) {
    Socket* client = nullptr;

    // Continuously process client requests until signaled to stop
    while (true) {
      client = server->clientQueue.pop(); // Pop a client from the client queue

      // Check if there is an error in popping the client or if the server is closing
      if ((int)(size_t)client == -1 || client == nullptr) {
        break;
      }

      client->SSLCreate(server->clientSocket); // Create an SSL context for the client
      client->SSLAccept(); // Perform SSL handshake with the client
      processClientRequest(client, server->legos); // Process the client request
    }
  }

  /**
   * Process a browser request by sending a response to the client with a table of lego figures and their amounts.
   * @param client A pointer to the Socket representing the client connection.
   * @param legos A constant reference to the map containing lego figures and their amounts.
   */
  static void processBrowserRequest (Socket* client,
      const std::map<std::string, size_t>& legos) {
    std::cout << "Serving browser request" << std::endl;
    std::string response =
        // send header
        "HTTP/1.0 200\r\n"
        "Content-type: text/html; charset=UTF-8\r\n"
        "Server: AttoServer v1.1\r\n"
        "\r\n"

        // send html format and title
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<html lang=\"en\">\n"
        "   <meta charset=\"UTF-8\"/>\n"
        "   <title>Figures Server Pieces List </title>\n"
        "   <style>body {font-family: monospace}</style>\n"
        "   <h1>Figures Server Pieces List</h1>\n"
        "<TABLE BORDER=1 BGCOLOR=\"pink\" CELLPADDING=5 ALIGN=LEFT>\n"
        "<TR> <TH> Cantidad </TH> <TH> Descripción </TH> </TR>\n";

    // add figures and their amounts to the table
    for (auto it = legos.begin();
        it != legos.end();
        it++) {
      if (it->second != 0) {
        response.append("<TR><TD ALIGN=center> ");
        response.append(std::to_string(it->second));
        response.append(
            "</TD>\n"
            "<TD ALIGN=center> ");
        response.append(it->first);
        response.append(
          "</TD>\n"
          "</TR>\n");
      }
    }

    // close table and html doc
    response.append(
        "</TR></TABLE>\n"
        "</html>");

    // send all bytes
    client->Write(
        response.c_str(),
        response.size()
        );
  }

  /**
   * Process a client request by checking if the requested lego pieces are available and responding with a success or failure message.
   * @param client A pointer to the Socket representing the client connection.
   * @param clientSocket A pointer to the Socket representing the server's client socket.
   * @param legos A reference to the map containing lego figures and their amounts.
   */
  static void processClientRequest (Socket* client,
      std::map<std::string, size_t>& legos) {

    char response[2];
    memset(response, 0, 2);
    response[0] = '0';

    char code[1];

    client->SSLRead(code, 1);
    
    std::vector<std::pair<std::string, size_t>> requestedPieces;

    if (code[0] == 0) {
      // recibir piezas
      processRequest(client, requestedPieces);
    } else {
      int bufferSize = 120;
      char nachosBuffer[bufferSize];
      memset(nachosBuffer, 0, bufferSize);
      int size = 0;

      // recibir piezas de nachos
      while (nachosBuffer[0] != 4) {
        client->SSLRead(nachosBuffer, bufferSize);

        if (nachosBuffer[0] != 4) {
          size_t amount = (size_t) std::stoi(nachosBuffer);
          memset(nachosBuffer, 0, bufferSize);
          
          client->SSLRead(nachosBuffer, bufferSize);
          std::string description = nachosBuffer;

          requestedPieces.push_back({description, amount});
          memset(nachosBuffer, 0, bufferSize);
        }
      } 
    }

    // amount of pieces found
    size_t piecesFountAmount = 0;

    // check if all pieces are available
    for (size_t piece = 0; piece < requestedPieces.size(); piece++) {
      if (legos[requestedPieces[piece].first] >=
          requestedPieces[piece].second) {
        piecesFountAmount++;
      }
    }

    // if available take out the pieces
    if (piecesFountAmount == requestedPieces.size()) {
      // check if all pieces are available
      for (size_t piece = 0; piece < requestedPieces.size(); piece++) {
        if (legos[requestedPieces[piece].first] != 0) {
          legos[requestedPieces[piece].first] -=
            requestedPieces[piece].second;
        }  
      }

      // set response to positive
      response[0] = '1';
    }

    // send all bytes
    client->SSLWrite(
        response,
        2
        );

  } 

  /**
   * Listens for incoming browser connections and queues them for further processing.
   * @param piecesServer A pointer to the PiecesServer instance.
   */
  static void listenBrowserConnections(PiecesServer* piecesServer) {
    Socket* client;

    // look for connection requests on socket
    while (true) {
      std::cout << "Listening to browser connections" << std::endl;

      // Accept a new browser connection
      client = piecesServer->browserSocket->Accept();

      // Check if the client connection is valid or if the server is closing
      if ((int)(size_t)client == -1 || client == nullptr || piecesServer->closing) {
        std::cout << "Ending browser connections thread" << std::endl;
        piecesServer->browserQueue.push(nullptr);
        break;
      }

      std::cout << "Browser connection accepted\n" << std::endl;

      // Queue the client connection for further processing
      piecesServer->browserQueue.push(client);
    }
  }

  /**
   * Processes the client request and extracts the requested pieces from the received data.
   * @param clientSocket The client socket from which the request is received.
   * @param requestedPieces A vector to store the extracted requested pieces.
   */
  static void processRequest(Socket* clientSocket,
      std::vector<std::pair<std::string, size_t>>& requestedPieces) {
    std::string response;
    char buffer[501];
    memset(buffer, 0, 501);
    std::string lastLine = "";
    std::string line = "";
    std::string endOfDoc = "";
    int cyclesSinceEndOfBytes = 4;
    int count = 0;

    while (clientSocket->SSLRead(buffer, 500) > 0) {
      count++;
      response.erase();
      response.resize(strlen(buffer));
      response = buffer;
      int bufferSize = strnlen(buffer, 500);
      memset(buffer, 0, sizeof(buffer));
      size_t character = 0;
      size_t initLocation = 0;

      bool endOfBytes = false;

      while (character < response.size()) {
        // check if at the end of bytes read (incomplete statement possible)
        endOfBytes = (character == response.size() - 1);

        // if whole line or end of bytes
        if(response[character] == '\n' || endOfBytes) {

          // if two turns gone by
          if (cyclesSinceEndOfBytes == 2) {
            // join saved lines with last line
            endOfDoc += lastLine;

            // set last line as all saved lines
            lastLine = endOfDoc;

            size_t emptyTries = 0;
            // empty previous buffer
            while(endOfDoc.empty() && emptyTries < 10) {
              emptyTries++;
            }

            // restart cycle out of range
            cyclesSinceEndOfBytes = 4;
          }

          int adjustment = 1;
          // adjust location of end of string if character is invalid
          if ((u_int)(char) response[character - 1] > 127) {
            adjustment-=2;
          }
          
          // if at end of bytes
          if (endOfBytes) {
            // copy previous and actual lines
            endOfDoc = lastLine +
                response.substr(initLocation, character - initLocation + adjustment);
            // begin counter of cycles since found
            cyclesSinceEndOfBytes = 0;
          }

          // analyze last line and this line together
          // (some regex may need to analyze two lines at once)
          line = lastLine +
              response.substr(initLocation, character - initLocation + adjustment);

          // run regex to analyze line
          regexAnalyzer(line, requestedPieces);

          // set current line as last line for the next iteration to have it
          lastLine = response.substr(initLocation, character - initLocation + adjustment);

          // increase counters
          initLocation = character + 1;
          cyclesSinceEndOfBytes++;
        }
        character++;
      }

      if ((unsigned int) response[response.size() - 1] == 4) {
        break;
      }   
    }
  }

  /**
   * @brief Analyzes a line of text using a regular expression to extract the quantity and description of a Lego piece.
   *
   * @param line The line of text to analyze.
   * @param requestedPieces A vector of pairs representing the requested Lego pieces and their quantities.
   */
  static void regexAnalyzer(std::string& line,
      std::vector<std::pair<std::string, size_t>>& requestedPieces) {
    // Regex to match pieces in figure response
    std::regex regexPiece("<TR><TD ALIGN=center?> (\\d+)</TD>\\s*<TD ALIGN=center> ([^<]+)</TD>");

    std::smatch pieza_match;
    std::string::const_iterator begin(line.cbegin());
    if (std::regex_search(begin, line.cend(), pieza_match, regexPiece)) {
      // Extract quantity and description of the piece
      std::string amount = pieza_match[1];
      std::string description = pieza_match[2];
      // Convert quantity to integer
      size_t cantidad = (size_t) std::stoi(amount);

      // add pair to the vector
      requestedPieces.push_back(
          {description, cantidad});

      // Update position in the response string
      begin = pieza_match.suffix().first;
    }
  }
};


#endif