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
        handlingData->responseVec.erase(handlingData->responseVec.begin());
        handlingData->responseVec.erase(handlingData->responseVec.begin());
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

#endif