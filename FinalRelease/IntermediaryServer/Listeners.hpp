#include "Generics/Listener.hpp"

class ClientListener : public Listener<std::shared_ptr<Socket>> {
 public:
  ClientListener (Queue<std::shared_ptr<Socket>>* queue,
      Socket* listeningSocket,
      std::shared_ptr<Socket> stopCondition,
      std::shared_ptr<Socket> handlerStopCondition,
      int stopPort,
      bool ssl,
      char socketType,
      std::string& listeningMessage, 
      std::string& afterReceivedMessage,
      std::string& stopMessage) : 
        Listener (
          queue,
          listeningSocket,
          stopCondition,
          handlerStopCondition,
          stopPort,
          ssl,
          socketType,
          listeningMessage,
          afterReceivedMessage,
          stopMessage) {}

 private:
  std::shared_ptr<Socket> obtain() {
    std::shared_ptr<Socket> receivedConnection = this->listeningSocket->Accept();
    receivedConnection->SSLCreate(this->listeningSocket);
    receivedConnection->SSLAccept();

    return receivedConnection;
  }
};

class UDPListener : public Listener<std::shared_ptr<std::vector<char>>> {
 public:
  UDPListener (Queue<std::shared_ptr<std::vector<char>>>* queue,
      Socket* listeningSocket,
      std::shared_ptr<std::vector<char>> stopCondition,
      std::shared_ptr<std::vector<char>> handlerStopCondition,
      int stopPort,
      bool ssl,
      char socketType,
      std::string& listeningMessage,
      std::string& afterReceivedMessage,
      std::string& stopMessage) : 
        Listener (
          queue,
          listeningSocket,
          stopCondition,
          handlerStopCondition,
          stopPort,
          ssl,
          socketType,
          listeningMessage,
          afterReceivedMessage,
          stopMessage) {}

 private:
  std::shared_ptr<std::vector<char>> obtain() {
    char buffer[100];
    memset(buffer, 0, 100);

    // get struct for udp
    struct sockaddr_in sockStruct;
    memset(&sockStruct, 0, sizeof(sockaddr_in));

    // set the port from where messages are expected
    sockStruct.sin_port = htons(INTERMEDIARY_UDP_PORT);
    // receive from any address
    sockStruct.sin_addr.s_addr = INADDR_ANY;

    int bytesRead = this->listeningSocket->recvFrom((void*)buffer, 100, &sockStruct);

    std::shared_ptr<std::vector<char>> vec = std::make_shared<std::vector<char>>();

    vec->resize(bytesRead);

    memcpy(&vec->data()[0], buffer, bytesRead);

    return vec;
  }
};