#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"

#include <regex>
#include <fstream>
#include <algorithm>

static const char* pageHeader = "<style>\n"
    "body {\n"
    "  text-align: center;\n"
    "  font-family: Arial, sans-serif;\n"
    "  font-size: 32px;\n"
    "  background-color: beige;\n"
    "}\n"
    "h1 {\n"
    "  font-size: 64px;\n"
    "}\n"
    "h1.colorful {\n"
    "  animation: rainbow 5s infinite linear;\n"
    "}\n"
    "@keyframes rainbow {\n"
    "  0% { color: red; }\n"
    "  14% { color: orange; }\n"
    "  28% { color: yellow; }\n"
    "  42% { color: green; }\n"
    "  57% { color: blue; }\n"
    "  71% { color: indigo; }\n"
    "  85% { color: violet; }\n"
    "  100% { color: red; }\n"
    "}\n"
    "select[name=\"figures\"] {\n"
    "  font-size: 24px;\n"
    "  width: 150px;\n"
    "}\n"
    "</style>\n"
    "</head>\n"
    "<body>\n"
    "<h1 class=\"colorful\">ESJOJO</h1>\n"
    "<div class=\"st12\">\n"
    "   <h1>¡Bienvenido al servidor intermedio!</h1>\n";

class RequestHandler : public Handler<std::shared_ptr<Request>>  {
  RoutingMap* routingMap;
  Queue<std::shared_ptr<Response>>* responseQueue;
 public:
  RequestHandler(Queue<std::shared_ptr<Request>>* consumingQueue,
      std::shared_ptr<Request> stopCondition, 
      RoutingMap* routingMap, 
      Queue<std::shared_ptr<Response>>* responseQueue)
          : Handler(consumingQueue, stopCondition)
          , routingMap(routingMap)
          , responseQueue(responseQueue){}

 private:
  void handleSingle(std::shared_ptr<Request> handlingData) { //HTTP
    // use request to find ip and port from the map
    serverAction requestType = handlingData->requestType;
    std::string figure = handlingData->figure;

    if (handlingData == nullptr) {
      std::cout << "Error!!!!" << std::endl;
    }

    std::string responseString;
    std::vector<char> responseVector;

    // send info to pieces server
    switch(requestType) {
      case serverAction::requestingFigures:
        // receive info from pieces server
        this->fillMainPage(responseString);
        break;

      case serverAction::requestingParts:
        // receive info from pieces server
        this->getParts(responseString, figure);
        break;

      case serverAction::requestingAssembly:
        {
        // for the case where the figure still exists
        std::string figureName = figure.substr(9, figure.size());

        if (this->routingMap->count(figureName) == 0) {
          this->notFound(responseString);
          break;
        }

        std::unique_ptr<Socket> piecesServerConnection =
            std::move(tryConnection(figureName));

        if (piecesServerConnection == nullptr) {
          // write response
          this->reportNotAssemble(responseString, figureName);
        // for the case where the figure does not exist
        } else {
          // communicate to prevent crash
          // receive info from pieces server
          std::string request;
          request.push_back(LEGO_REQUEST);
          request.push_back(SEPARATOR);
          request += figure;

          *piecesServerConnection << request;

          std::string buffer;
          
          *piecesServerConnection >> buffer;

          // write response
          this->reportAssembled(responseString, figureName);
          
          piecesServerConnection->Close();
        }
        // receive info from pieces server
        }

        break;

      case serverAction::requestingImage:
        this->requestImage(figure, responseString, responseVector);
        break;

      case serverAction::requestingBrowserIcon:
        sendIcon(responseVector);
        break;

      default:
        break;
    }

    //enqueue response
    std::shared_ptr<Response> response = std::make_shared<Response>(
        handlingData->socket,
        responseString,
        handlingData->requestType,
        responseVector
        );

    this->responseQueue->push(response);
  }

  void sendIcon(std::vector<char>& responseVector) {
    std::fstream icon;
    icon.open("chickiIcon.ico", std::ios::binary | std::ios::in);

    if (!icon.is_open()) {
      std::cout << "could not open!" << std::endl;
      return;
    }

    char buffer[1406];

    int size = 0;

    while(icon.read(buffer, 1406)) {
      int currentSize = icon.gcount();
      size += currentSize;

      int initPos = responseVector.size();

      responseVector.resize(responseVector.size() + currentSize);
      memcpy(&responseVector[initPos], buffer, currentSize);
    }

    icon.close();
  }

  void requestImage(std::string& figure, std::string& response, std::vector<char>& responseVector) {
    int pos = figure.size() - 1;

    while (figure[pos] != '/' && pos != 0) {
      pos--;
    }

    int end = figure.size() - 1;

    while (figure[end] != '.' && end != 0) {
      end--;
    }

    if (pos == 0 || end == 0) {
      this->notFound(response);
      return;
    }

    std::string::difference_type amountN = std::count(figure.begin(), figure.end(), '/');

    int amount = int(amountN);

    std::string figureBuffer;

    if (amount > 1) {
      figureBuffer = figure.substr(0, figure.find('/'));
      figure = figure.substr(figure.find('/') + 1, figure.size());
    } else {
      figureBuffer = figure.substr(pos + 1, end - pos - 1);
    }

    if (this->routingMap->count(figureBuffer) == 0) {
      this->notFound(response);
      return;
    }

    std::unique_ptr<Socket> piecesServerConnection =
        std::move(tryConnection(figureBuffer)); // check for nullptr

    if (piecesServerConnection == nullptr) {
      this->notFound(response);
      return;
    }

    int imageBufferSize = 16384; // 100 kb lol

    piecesServerConnection->setBufferDefault(imageBufferSize);

    // receive info from pieces server
    std::string request;
    request.push_back(LEGO_REQUEST);
    request.push_back(SEPARATOR);
    request += figure;

    *piecesServerConnection << request;

    std::vector<char> responseBuffer;

    int bytesRead = 0;

    // just receive the stream of bytes in raw
    while ((bytesRead = (*piecesServerConnection >> responseBuffer)) > 0) {
      responseVector = std::move(responseBuffer);
      if (bytesRead != imageBufferSize) {
        break;
      }
    }
    piecesServerConnection->Close();
  }

  void notFound(std::string& response) {
    response +=
        "<SCRIPT LANGUAGE=javascript>\n"
        "function home() {\n"
          "window.location( \"/lego/index.php\" );\n"
        "}\n"
        "</SCRIPT>\n";

    response +=
        "<DIV class=\"st10\">"
        "<TABLE WIDTH=100%>\n";
    response += pageHeader;

    response += 
        "<HR>\n"
        "<CENTER><H2>Not Found - Error 404</H2></CENTER>\n";

    response +=
      "<TR>\n"
      "<TD> <A HREF=\"/lego/index.php\"> Regresar </A>\n"
      "</TR>\n";

    response += "</TABLE>\n";
  }


  void reportAssembled (std::string& response, std::string figure) {

    response +=
        "<DIV class=\"st10\">"
        "<TABLE WIDTH=100%>\n";
    response += pageHeader;

    response += 
        "<HR>\n"
        "<CENTER><H2>" + figure + " ha sido armada</H2></CENTER>\n";

    response +=
      "<TR>\n"
      "<TD> <A HREF=\"/lego/index.php\"> Regresar </A>\n"
      "</TR>\n";

    response += "</TABLE>\n";
  }


  void reportNotAssemble(std::string& response, std::string figure) {
    response +=
        "<SCRIPT LANGUAGE=javascript>\n"
        "function home() {\n"
          "window.location( \"/lego/index.php\" );\n"
        "}\n"
        "</SCRIPT>\n";

    response +=
        "<DIV class=\"st10\">"
        "<TABLE WIDTH=100%>\n";
    response += pageHeader;

    response += 
        "<HR>\n"
        "<CENTER><H2>" + figure + " no ha sido armada</H2></CENTER>\n";

    response +=
      "<TR>\n"
      "<TD> <A HREF=\"/lego/index.php\"> Regresar </A>\n"
      "</TR>\n";

    response += "</TABLE>\n";
  }

  void getParts(std::string& response, std::string figure) {
    if (this->routingMap->count(figure) == 0) {
      this->notFound(response);
      return;
    }

    std::unique_ptr<Socket> piecesServerConnection =
        std::move(tryConnection(figure)); // check for nullptr

    if (piecesServerConnection == nullptr) {
      this->notFound(response);
      return;
    }

    piecesServerConnection->setBufferDefault(8096);

    // receive info from pieces server
    std::string request;
    request.push_back(LEGO_REQUEST);
    request.push_back(SEPARATOR);
    request += figure;

    *piecesServerConnection << request;

    // 
    response +=
        "<SCRIPT LANGUAGE=javascript>\n"
        "function home() {\n"
          "window.location( \"/lego/index.php\" );\n"
        "}\n"
        "</SCRIPT>\n";

    response +=
        "<SCRIPT LANGUAGE=javascript>\n"
        "function assemble() {\n"
          "window.location( \"/assemble\" );\n"
        "}\n"
        "</SCRIPT>\n";

    response += 
        "<DIV class=\"st10\">"
        "<TABLE WIDTH=100%>\n";
    response += pageHeader;

    response += 
        "<HR>\n"
        "<CENTER><H2>" + figure + "</H2></CENTER>\n";

    response +=
      "<TR>\n"
      "<TD> <A HREF=\"/lego/index.php\"> Regresar </A>\n"
      "</TR>\n";

    response +=
      "<TR>\n"
      "<TD> <A HREF=\"/assemble/";

    response += figure;

    response +=
      "\"> Armar </A>\n"
      "</TR>\n";
    
    response += "</TABLE>\n";

    std::string responseBuffer;

    while ((*piecesServerConnection >> responseBuffer) == 8096) {
    }

    response += responseBuffer;

    piecesServerConnection->Close();
  }

  void fillMainPage(std::string& response) {
    response += "<script>\n"
    "function goFigure(form) {\n"
    "   figure = form.figures.options[form.figures.selectedIndex].value;\n"
    "   form.figures.selectedIndex = 0;\n"
    "   if (figure === 'caballo') {\n"
    "       location.href = '/lego/horse.php';\n"
    "   } else {\n"
    "       location.href = '/lego/list.php?figure=' + figure;\n"
    "   }\n"
    "}\n"
    "</script>\n";

    response += pageHeader;

    response += "   <form>\n"
    "      <select name=\"figures\" onchange=\"goFigure(this.form)\">\n";

    response +=  "<option value=\"None\"> Elegir </option>\n";

    for (auto figure : *(this->routingMap)) {
      response += "         <option value=\"";
      response += figure.first;
      response += "\"> ";
      response += figure.first;
      response += " </option>\n";
    }

    response +=
    "      </select>\n"
    "   </form>\n";
  }

  void optionalToEnd () {
    std::cerr << "request handler dying" << std::endl;
    this->responseQueue->push(nullptr);
  }

  std::unique_ptr<Socket> tryConnection (std::string& figure) {
    if (this->routingMap->count(figure) == 0) {
      return nullptr;
    }

    std::unique_ptr<Socket> piecesServerSocket = std::make_unique<Socket>('s', false);
    piecesServerSocket->InitSSL();

    std::string ip = (*this->routingMap)[figure].first;
    int port = (*this->routingMap)[figure].second;

    // set timeout
    piecesServerSocket->increaseTimeout(FIRST_TIMEOUT);
    // first try
    int connectionResult = piecesServerSocket->SSLConnect(ip.data(), port);
    switch(connectionResult) {
      case 1:
          return piecesServerSocket;
        break;
      case ECONNREFUSED:
        this->routingMap->erase((*this->routingMap)[figure]);
        return nullptr;
    }

    // increase timeout
    piecesServerSocket->increaseTimeout(SECOND_TIMEOUT);
    // second try
    connectionResult = piecesServerSocket->SSLConnect(ip.data(), port);
    switch(connectionResult) {
      case 1:
          return piecesServerSocket;
        break;
      case ECONNREFUSED:
        this->routingMap->erase((*this->routingMap)[figure]);
        return nullptr;
    }

    // increase timeout
    piecesServerSocket->increaseTimeout(THIRD_TIMEOUT);
    // final try
    connectionResult = piecesServerSocket->SSLConnect(ip.data(), port);
    switch(connectionResult) {
      case 1:
          return piecesServerSocket;
        break;
      case ECONNREFUSED:
        break;
    }

    this->routingMap->erase((*this->routingMap)[figure]);
    // report failure
    return nullptr;
  }
};


#endif