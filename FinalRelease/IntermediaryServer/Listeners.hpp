#include "Generics/Listener.hpp"

class ClientListener : public Listener<Socket*> {
 public:
  ClientListener (Queue<Socket*>* queue,
      Socket* listeningSocket,
      Socket* stopCondition,
      Socket* handlerStopCondition,
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
  Socket* obtain() {
    Socket* receivedConnection = this->listeningSocket->Accept();
    receivedConnection->SSLCreate(this->listeningSocket);
    receivedConnection->SSLAccept();
    return receivedConnection;
  }
};

class UDPListener : public Listener<std::shared_ptr<std::pair<std::string, int>>> {
 public:
  UDPListener (Queue<std::shared_ptr<std::pair<std::string, int>>>* queue,
      Socket* listeningSocket,
      std::shared_ptr<std::pair<std::string, int>> stopCondition,
      std::shared_ptr<std::pair<std::string, int>> handlerStopCondition,
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
  std::shared_ptr<std::pair<std::string, int>> obtain() {
    char buffer[100];
    memset(buffer, 0, 100);

    // get struct for udp
    struct sockaddr_in sockStruct;
    memset(&sockStruct, 0, sizeof(sockaddr_in));

    // set the port from where messages are expected
    sockStruct.sin_port = htons(this->stopPort);
    // receive from any address
    sockStruct.sin_addr.s_addr = INADDR_ANY;

    int bytesRead = this->listeningSocket->recvFrom((void*)buffer, 100, &sockStruct);

    int byte = 4;

    std::string ip;

    // interpret (4 bytes for code, 1 for separator, next ip until port)
    for (; byte < bytesRead; byte++) {
      if (buffer[byte] == 29) {
        break;
      }
      ip.push_back(buffer[byte]);
    }

    int port = 0;

    // last 4 usable bytes are port
    memcpy(&port, &buffer[byte], 4);

    // send pair with information
    return std::make_shared<std::pair<std::string, int>>(ip, port);
  }
};