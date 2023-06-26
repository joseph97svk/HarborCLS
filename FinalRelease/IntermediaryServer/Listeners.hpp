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
        return nullptr;
    }
};

class TCPListener : public Listener {
 public:
    TCPListener (Queue<Socket*>* queue,
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