#include "Generics/Handler.hpp"

// handle connections from the intermediary client
class TCPHandler : public Handler {
 public:
  TCPHandler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(Socket* handlingSocket) {
    (void) handlingSocket;
  }
};

// handle connections from broadcasts
class UDPHandler : public Handler {
 public:
  UDPHandler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(Socket* handlingSocket) {
    (void) handlingSocket;
  }
};