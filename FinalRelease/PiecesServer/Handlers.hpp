#include "Generics/Handler.hpp"
#include <utility>

// handle connections from the intermediary client
class TCPHandler : public Handler<Socket*> {
 public:
  TCPHandler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(Socket* handlingSocket) {
    (void) handlingSocket;
  }
};

// handle connections from broadcasts
class UDPHandler : public Handler<std::pair<std::string, int>> {
 public:
  UDPHandler(Queue<std::pair<std::string, int>>* consumingQueue,
      std::pair<std::string, int> stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(std::pair<std::string, int> handlingSocket) {
    std::cout << "ip address: " << handlingSocket.first <<
        "\nport:" << handlingSocket.second << std::endl;
  }
};