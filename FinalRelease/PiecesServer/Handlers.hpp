#include "Generics/Handler.hpp"
#include "Generics/ProtocolHeader.hpp"

#include <utility>
#include <memory>
#include "PiecesServerStructure.hpp"
// handle connections from the intermediary client
class TCPHandler : public Handler<std::shared_ptr<Socket>> {
 private:
  LegoMap* legoMap;
 public:
  TCPHandler(Queue<std::shared_ptr<Socket>>* consumingQueue,
      std::shared_ptr<Socket> stopCondition, LegoMap* legoMap) : Handler (consumingQueue, stopCondition) {
        this->legoMap = legoMap;
      }

 private:
  void handleSingle(std::shared_ptr<Socket> handlingData) {
    (void) handlingData;
    std::string figure;
    std::string response;
    response = "";
    *handlingData << response;
  }
};

// handle connections from broadcasts
class UDPHandler : public Handler<std::shared_ptr<std::vector<char>>> {
 public:
  UDPHandler(Queue<std::shared_ptr<std::vector<char>>>* consumingQueue,
      std::shared_ptr<std::vector<char>> stopCondition) : Handler (consumingQueue, stopCondition){}

 private:
  void handleSingle(std::shared_ptr<std::vector<char>> handlingData) {
    LegoMessageCode code = (LegoMessageCode) 0;

    // get the code from the message
    memcpy(&code, (void*) handlingData->data(), sizeof(LegoMessageCode));
    
    std::string buffer;

    buffer.resize(handlingData->size() - 5);

    memcpy(buffer.data(), &handlingData->data()[5], buffer.size());

    // get the host
    std::string ip = buffer.substr(
        0, // from start
        buffer.find(':') - 1 // until separator
        );

    // get the port
    int port = std::stoi(buffer.substr(
        buffer.find(':') + 1, // from after separator
        buffer.size() // until end of message
        ));

    std::cout << "CODE: " << code
        << "\n\tip address: " << ip
        << "\n\tport:" << port << std::endl;
  }
};