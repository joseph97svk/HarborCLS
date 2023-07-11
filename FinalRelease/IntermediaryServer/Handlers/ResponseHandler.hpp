#ifndef RESPONSEHANDLER_HPP
#define RESPONSEHANDLER_HPP

#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"

#include <regex>
#include <fstream>
#include <algorithm>

// for handling the reponse to be sent to the client
class ResponseHandler : public Handler<std::shared_ptr<Response>>  { //Se encarga de las respuestas al cliente
 public:
  ResponseHandler(Queue<std::shared_ptr<Response>>* consumingQueue,
      std::shared_ptr<Response> stopCondition)
          : Handler(consumingQueue, stopCondition) {}

 private:
  void handleSingle(std::shared_ptr<Response> handlingData) {

    if (handlingData->requestType == serverAction::requestingImage ||
        handlingData->requestType == serverAction::requestingBrowserIcon) {
      if (handlingData->response == "404") {
        *handlingData->socket << handlingData->response;
        handlingData->socket->Close();
        return;
      }

      // send header
      std::string response =
        "HTTP/1.1 200 OK\r\n";

      if (handlingData->requestType == serverAction::requestingBrowserIcon) {
        response +=  
        "Content-Type: image/png\r\n";
      } else {
        response +=  
        "Content-Type: image/x-icon\r\n";
      }

      response += "Content-Transfer-Enconding: binary\r\n"
        "Content-Length: ";

      response += std::to_string(handlingData->responseVec.size());

      response += "\r\n"
        "\r\n";

      std::vector<char> responseVector;
      responseVector.resize(response.size() + handlingData->responseVec.size());
      memcpy(&responseVector[0], response.data(), response.size());
      memcpy(&responseVector[response.size()], handlingData->responseVec.data(), handlingData->responseVec.size());

      handlingData->socket->setBufferDefault(100000);
      *handlingData->socket << responseVector;

      handlingData->socket->Close();
      return;
    }

    std::string response =
      // send header
      "HTTP/1.1 200 OK\r\n"
      "Content-type: text/html; charset=UTF-8\r\n"
      "Server: AttoServer v1.1\r\n"
      "\r\n"
      // send html format and title
      "<!DOCTYPE html>\n"
      "<html>\n"
      "<head>\n";

    response += handlingData->response;

    response +=
      "</div>\n"
      "</body>\n"
      "</html>\n";

    response += "</h1></body></html>";
  
    *handlingData->socket << response;
    handlingData->socket->Close();
  }

  void optionalToEnd () {
    std::cerr << "response handler dying" << std::endl;
  }
};

// handle all broadcasts received
class UDPHandler : public Handler<std::shared_ptr<std::vector<char>>> {
 private:
  RoutingMap* routingMap;
 public:
  UDPHandler(Queue<std::shared_ptr<std::vector<char>>>* consumingQueue,
      std::shared_ptr<std::vector<char>> stopCondition,
      RoutingMap* routingMap)
          : Handler(consumingQueue, stopCondition)
          , routingMap(routingMap){}

 private:
  // socket of client to send response
  void handleSingle(std::shared_ptr<std::vector<char>> handlingData) {
    std::string buffer;

    buffer.push_back((*handlingData)[0]);

    std::string a;

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

    switch(code) {
      case LEGO_PRESENT:
         // insert figures
        this->insertFigures(buffer, ip, port);
        break;
      case LEGO_RELEASE:
        // remove figures asociated with server
        std::pair<std::string, int> piecesServerInfo = {ip, port};
        this->routingMap->erase(piecesServerInfo);
        break;
    }
  }

  void insertFigures(std::string& buffer, const std::string& ip, const int port) {
    std::string figure;

    size_t nextFigure = 0,
        end = 0;

    // remove everything before figure
    nextFigure = buffer.find(SEPARATOR);

    buffer = buffer.substr(nextFigure, buffer.size());

    do {
      
      // find next divisor
      nextFigure = buffer.find(SEPARATOR) + 1;

      // if no figure
      if (nextFigure == std::string::npos) {
        // exit and do nothing
        break;
      }

      // check for end of figure
      end = buffer.substr(1, buffer.size()).find(SEPARATOR);

      // if no separator then the end is the last char
      if (end == std::string::npos) {
        end = buffer.size();
      }

      // get figure
      std::string figure = buffer.substr(nextFigure, end);

      // insert the figure and the info 
      this->routingMap->insert({figure, {ip, port}});

      // remove figure
      buffer = buffer.substr(end + 1, buffer.size());

    // as long as the end is not the end of the everything
    } while (end != buffer.size() && buffer.size() > 1);
  }

  void optionalToEnd () {
    std::cerr << "udp handler dying" << std::endl;
  }
};


#endif