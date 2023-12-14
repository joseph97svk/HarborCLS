#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "../../HttpServer/Handler.hpp"
#include "../../HttpServer/ProtocolHeader.hpp"

#include <utility>
#include <memory>
#include "PiecesServerStructure.hpp"

#include "common.hpp"
#include "Handler.hpp"

#include <fstream>
#include <regex>

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
    std::string response;

    std::string figure;

    std::vector<char> buffer;

    int bytesReceived = 0;

    do {
      bytesReceived = (*handlingData >> buffer);
    } while (bytesReceived == 500 && bytesReceived > 0);

    figure.resize(buffer.size() - 2);
    memcpy(figure.data(), &(buffer.data()[2]), buffer.size() - 2);
    
    // in case it is an image request, get the image
    if (getImage(figure, handlingData)) {
      return;
    }

    if (this->legoMap->count(figure) == 0) {
      response = "suuuuuuuuppppp";
      *handlingData << response;
      return;
    }

    std::string figureImage = (*this->legoMap)[figure].first;
    std::vector<Lego>& legos = (*this->legoMap)[figure].second;

    response.push_back(std::to_string(LegoMessageCode::LEGO_RESPONSE)[0]);
    response.push_back(SEPARATOR);

    size_t totalAmount = 0;
    response +=
        "<TABLE BORDER=1 BGCOLOR=\"lightblue\" CELLPADDING=5 ALIGN=CENTER>\n"
        "<TR><TH>Cantidad</TH><TH>Descripción</TH><TH>Imagen</TH></TR>\n";

    for (auto& lego : legos) {
      response += 
          "<TR><TD ALIGN=center> " + std::to_string(lego.amount) + "</TD>\n"
          "<TD ALIGN=center> " + lego.description + "</TD>\n"
          "<TD ALIGN=center><IMG SRC=\"" + lego.imageFigure + "\" width=100 height=100></TD></TR>\n";
      totalAmount += lego.amount;
    }

    response += 
        "<TR><TD COLSPAN=3><IMG SRC=\"" + figureImage + "\" width=500 height=500></TD></TR>\n"
        "<TR><TD COLSPAN=2>Total de piezas para armar esta figura</TD>"
        "<TD ALIGN=center><H2>" + std::to_string(totalAmount) + "</H2></TD></TR>\n"
        "</TABLE>";
    *handlingData << response;
  }

  bool getImage(std::string& buffer, std::shared_ptr<Socket> connection) {
    std::string response;
  
    if (isImage(buffer)) {
      int pos = buffer.size() - 1;

      while (buffer[pos] != '/' && pos != 0) {
        pos--;
      }

      int end = buffer.size() - 1;

      while (buffer[end] != '.' && end != 0) {
        end--;
      }

      if (pos == 0 || end == 0) {
        // set response as not found
        response = "404";

        // send response back
        *connection << response;

        // no further task to do
        return false;
      }

      // get the image
      std::string figureBuffer = buffer.substr(pos + 1, end - pos - 1);

      if (!sendImage(buffer, connection)) {
        std::string response  = "404";
        *connection << response;
      }

      return true;
    }

    return false;
  }

  bool isImage(std::string& buffer) {
    std::regex findImage("lego/");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findImage)) {
      return true;
    }
    return false;
  }

  bool sendImage(std::string& path, std::shared_ptr<Socket> connection) {
    std::fstream imageFile;

    imageFile.open(path, std::ios::binary | std::ios::in);

    if (!imageFile.is_open()) {
      std::cout << "not opened" << std::endl;
      return false;
    }

    int bufferSize = connection->getBufferSize();
    int newBufferSize = 50000;
    connection->setBufferDefault(newBufferSize);

    char buffer[32];

    std::vector<char> message;

    message.push_back(std::to_string(LegoMessageCode::LEGO_RESPONSE)[0]);
    message.push_back(SEPARATOR);

    while(imageFile.read(buffer, 32)) {
      int currentSize = imageFile.gcount();

      int initPos = message.size();

      message.resize(message.size() + currentSize);
      memcpy(&message[initPos], buffer, currentSize);
    }

    connection->setBufferDefault(bufferSize);

    *connection << message;

    imageFile.close();
    return true;
  }
  void optionalToEnd () {}

};

// handle connections from broadcasts
class UDPHandler : public Handler<std::shared_ptr<std::vector<char>>> {
  LegoMap* piecesServerFigures;
 public:
  UDPHandler(
      Queue<std::shared_ptr<std::vector<char>>>* consumingQueue,
      std::shared_ptr<std::vector<char>> stopCondition,
      LegoMap* piecesServerFigures)
      : Handler (consumingQueue, stopCondition)
      , piecesServerFigures(piecesServerFigures) {}

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

    // send data back to int server
    this->sendUnicastResponse(ip, port);
  }

  void sendUnicastResponse(std::string& ip, int port) {
    std::vector<char> message;

    message.push_back(std::to_string(LegoMessageCode::LEGO_PRESENT)[0]);

    message.push_back(SEPARATOR);

    std::string buffer = getComputerIp();

    for (char character : buffer) {
      message.push_back(character);
    }

    message.push_back(':');

    buffer = std::to_string(PIECES_TCP_PORT).data();

    for (char character : buffer) {
      message.push_back(character);
    }

    message.push_back(SEPARATOR);

    // for all figures
    for (auto& figure : *(this->piecesServerFigures)) {
      // add the figure name
      for (char character : figure.first) {
        // add it to the message
        message.push_back(character);
      }

      // add separator
      message.push_back(SEPARATOR);
    }

    Socket unicastSocket('d', false);

    unicastSocket(ip, port) << message;
  }
  void optionalToEnd () {}
};

#endif