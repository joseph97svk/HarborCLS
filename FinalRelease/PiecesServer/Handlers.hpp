#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include "Generics/Handler.hpp"
#include "Generics/ProtocolHeader.hpp"

#include <utility>
#include <memory>
#include "PiecesServerStructure.hpp"

#include "common.hpp"

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

    while ((*handlingData >> buffer) == 500) {
    }

    figure.resize(buffer.size() - 2);
    memcpy(figure.data(), &(buffer.data()[2]), buffer.size() - 2);
    
    // in case it is an image request, get the image
    if (getImage(figure, handlingData)) {
      return;
    }

    std::string figureImage = (*this->legoMap)[figure].first;
    std::vector<Lego>& legos = (*this->legoMap)[figure].second;
    size_t totalAmount = 0;
    response =
        "<TABLE BORDER=1 BGCOLOR=\"lightblue\" CELLPADDING=5 ALIGN=CENTER>\n"
        "<TR><TH>Cantidad</TH><TH>Descripción</TH><TH>Imagen</TH></TR>\n";

    for (auto& lego : legos) {
      response += 
          "<TR><TD ALIGN=center>" + std::to_string(lego.amount) + "</TD>\n"
          "<TD ALIGN=center>" + lego.description + "</TD>\n"
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

        std::cout << "Invalid image request!" << std::endl;

        // no further task to do
        return false;
      }

      // get the image
      std::string figureBuffer = buffer.substr(pos + 1, end - pos - 1);

      std::cout << buffer << std::endl;

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
      return false;
    }

    int bufferSize = connection->getBufferSize();
    int newBufferSize = 50000;
    connection->setBufferDefault(newBufferSize);

    char buffer[512];

    std::vector<char> message;

    std::fstream newFile;
    newFile.open("newAwa.jpg", std::ios_base::out);

    int size = 0;

    while(imageFile.read(buffer, 512)) {
      int currentSize = imageFile.gcount();
      size += currentSize;

      int initPos = message.size();

      message.resize(message.size() + currentSize);
      memcpy(&message[initPos], buffer, currentSize);
    }

    newFile.write(message.data(), size);

    connection->setBufferDefault(bufferSize);
  
    *connection << message;

    newFile.close();
    imageFile.close();
    return true;
  }
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
    this->sendUnicastResponse();
  }

  void sendUnicastResponse() {
    std::vector<char> broadcastMessage;

    // for all figures
    for (auto& figure : *(this->piecesServerFigures)) {
      // add the figure name
      for (char character : figure.first) {
        // add it to the message
        broadcastMessage.push_back(character);
      }

      // add separator
      broadcastMessage.push_back(SEPARATOR);
    }

    broadcast(broadcastMessage, LEGO_PRESENT);
  }
};

#endif