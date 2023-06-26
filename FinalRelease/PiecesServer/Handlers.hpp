#include "Generics/Handler.hpp"

class TCPHandler : public Handler {
 public:
  TCPHandler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(Socket* handlingSocket) {
    (void) handlingSocket;
  }
};

class UDPHandler : public Handler {
 public:
  UDPHandler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(Socket* handlingSocket) {
    (void) handlingSocket;
  }
};