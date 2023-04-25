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

 public:
  PiecesServer(std::string legoSourceFileName = "",
    char type = 's',
    bool ipv6 = false);

  int readLegoSourceFile(std::string legoSourceFileName = "");

  void startServer();

 private:
  static void processBrowserRequests (PiecesServer* server) {
    Socket* client = nullptr;

    while (true) {
      client = server->browserQueue.pop();

      if (client == nullptr) {
        break;
      }

      processBrowserRequest(client, server->legos);
    }      
  }

  static void processClientRequests (PiecesServer* server) {
    Socket* client = nullptr;

    while (true) {
      client = server->clientQueue.pop();

      if (client == nullptr) {
        break;
      }

      processClientRequest(client, server->browserSocket, server->legos);
    }      
  }

  static void processBrowserRequest (Socket* client,
      const std::map<std::string, size_t>& legos) {

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
        "<TABLE BORDER=1 BGCOLOR=\"lightblue\" CELLPADDING=5 ALIGN=LEFT>\n"
        "<TR> <TH> Cantidad </TH> <TH> Descripción </TH> </TR>\n";

    // add figures and their amounts to the table
    for (auto it = legos.begin();
        it != legos.end();
        it++) {
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

  static void processClientRequest (Socket* client, Socket* browserSocket,
      std::map<std::string, size_t>& legos) {
    
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
        "<TABLE BORDER=1 BGCOLOR=\"lightblue\" CELLPADDING=5 ALIGN=LEFT>\n"
        "<TR> <TH> Cantidad </TH> <TH> Descripción </TH> </TR>\n";

    // add figures and their amounts to the table
    for (auto it = legos.begin();
        it != legos.end();
        it++) {
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

  static void listenBrowserConnections(PiecesServer* piecesServer) {
    Socket* client;

    // look for connection requests on socket
    while (true) {
      std::cout << "Listening to browser connections" << std::endl;

      client = piecesServer->browserSocket->Accept();
      std::cout << "Browser connection accepted\n" << std::endl;

      if ((int)(size_t)client == -1) {
        piecesServer->browserQueue.push(nullptr);
        break;
      }

      // queue the requests
      piecesServer->browserQueue.push(client);
    }
  }
};



#endif