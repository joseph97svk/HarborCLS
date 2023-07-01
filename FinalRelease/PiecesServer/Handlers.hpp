#include "Generics/Handler.hpp"
#include "Generics/ProtocolHeader.hpp"

#include <utility>
#include <memory>

// handle connections from the intermediary client
class TCPHandler : public Handler<std::shared_ptr<Socket>> {
 public:
  TCPHandler(Queue<std::shared_ptr<Socket>>* consumingQueue,
      std::shared_ptr<Socket> stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(std::shared_ptr<Socket> handlingData) {
    (void) handlingData;
  }
};

// handle connections from broadcasts
class UDPHandler : public Handler<std::shared_ptr<std::vector<char>>> {
 public:
  UDPHandler(Queue<std::shared_ptr<std::vector<char>>>* consumingQueue,
      std::shared_ptr<std::vector<char>> stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(std::pair<std::string, int> handlingSocket) {
    std::cout << "ip address: " << handlingData->first <<
        "\nport:" << handlingData->second << std::endl;
  }
};