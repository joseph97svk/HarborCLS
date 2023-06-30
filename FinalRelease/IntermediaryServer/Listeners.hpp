#include "Generics/Listener.hpp"

class ClientListener : public Listener {
 public:
  ClientListener (Queue<Socket*>* queue,
      Socket* listeningSocket,
      Socket* stopCondition,
      Socket* handlerStopCondition,
      int stopPort,
      bool ssl,
      char socketType,
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
          afterReceivedMessage,
          stopMessage) {}

 private:
  Socket* obtain() {
    std::cout << "accepting" << std::endl;
    Socket* receivedConnection = this->listeningSocket->Accept();
    // receivedConnection->SSLCreate(this->listeningSocket);
    // receivedConnection->SSLAccept();
    std::cout << "accepted" << std::endl;
    return receivedConnection;
  }
};

class UDPListener : public Listener {
 public:
  UDPListener (Queue<Socket*>* queue,
      Socket* listeningSocket,
      Socket* stopCondition,
      Socket* handlerStopCondition,
      int stopPort,
      bool ssl,
      char socketType,
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
          afterReceivedMessage,
          stopMessage) {}

 private:
  Socket* obtain() {
    return nullptr;
  }
};