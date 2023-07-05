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
    std::string figureImage = (*this->legoMap)[figure].first;
    std::vector<Lego>& legos = (*this->legoMap)[figure].second;
    size_t totalAmount = 0;
    std::string response = "<HR>\n"
                           "<CENTER><H2>Lista de piezas para armar la figura del final</H2></CENTER>\n"
                           "<TABLE BORDER=1 BGCOLOR=\"lightblue\" CELLPADDING=5 ALIGN=CENTER>\n"
                           "<TR><TH>Cantidad</TH><TH>Descripción</TH><TH>Imagen</TH></TR>\n";

    for (auto& lego : legos) {
      response += "<TR><TD ALIGN=center>" + std::to_string(lego.amount) + "</TD>\n"
                  "<TD ALIGN=center>" + lego.description + "</TD>\n"
                  "<TD ALIGN=center><IMG SRC=\"" + lego.imageFigure + "\" width=100 height=100></TD></TR>\n";
      totalAmount += lego.amount;
    }

    response += "<TR><TD COLSPAN=3><IMG SRC=\"" + figureImage + "\" width=500 height=500></TD></TR>\n"
                "<TR><TD COLSPAN=2>Total de piezas para armar esta figura</TD>"
                "<TD ALIGN=center><H2>" + std::to_string(totalAmount) + "</H2></TD></TR>\n"
                "</TABLE>";
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
    std::string buffer;

    buffer.push_back((*handlingData)[0]);

    int code = std::stoi(buffer);

    buffer.clear();

    buffer.resize(handlingData->size() - 2);

    memcpy(buffer.data(), &handlingData->data()[2], buffer.size());

    // get the host
    std::string ip = buffer.substr(
        0, // from start
        buffer.find(':') // until separator
        );

    // get the port
    int port = std::stoi(buffer.substr(
        buffer.find(':') + 1, // from after separator
        buffer.size() // until end of message
        ));

    std::cout << std::endl << "CODE: " << code
        << "\n\tip address: " << ip
        << "\n\tport:" << port << std::endl << std::endl;
  }
};