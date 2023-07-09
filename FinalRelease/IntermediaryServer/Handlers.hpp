#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"

#include <regex>

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

// for handling whatever the client sent
class ClientHandler : public Handler <std::shared_ptr<Socket>> {
 private:
  Queue<std::shared_ptr<Request>>* requestQueue;

 public:
  ClientHandler(Queue<std::shared_ptr<Socket>>* consumingQueue,
      Queue<std::shared_ptr<Request>>* requestQueue,
      std::shared_ptr<Socket> stopCondition)
          : Handler(consumingQueue, stopCondition)
          , requestQueue (requestQueue){}

 private:
  // socket of client from which request is to be read
  void handleSingle(std::shared_ptr<Socket> handlingData) { //Esta chuncha ya está conectada
    std::string buffer;

    while ((*handlingData >> buffer) == 2048) {
    }

    std::regex findHttp("(.* )HTTP/1.1");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    std::string httpRequest;

    if (std::regex_search(begin, buffer.cend(), requestMatch, findHttp)) {
      httpRequest = requestMatch[1];
    }

    // default is asking for figures
    serverAction action = serverAction::requestingFigures;

    // if asking for icon
    if (iconRequest(httpRequest)) {
      action = serverAction::requestingBrowserIcon;
    // for all else, as long as not empty request
    } 
    
    if (httpRequest != " " && !isMainPage(httpRequest)) {
      // for requesting parts
      if (assembleFigure(httpRequest)) {
        action = serverAction::requestingAssembly;
      // for assembling figure
      } else {
        action = serverAction::requestingParts;
      }

      size_t figureStartingPos = httpRequest.find('=') + 1;
      httpRequest = httpRequest.substr(figureStartingPos, httpRequest.size());

      httpRequest.pop_back();
    }

    if (getImage(httpRequest)) {
      action = serverAction::requestingImage;
    } 

    std::shared_ptr<Request> request = std::make_shared<Request>(handlingData, httpRequest, action);

    this->requestQueue->push(request);
  }

  void optionalToEnd () {
    this->requestQueue->push(nullptr);
    std::cerr << "Client handler dying" << std::endl;
  }

  bool iconRequest(std::string& buffer) {
    std::regex findIcon("favicon.ico");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findIcon)) {
      return true;
    }
    return false;
  }

  bool isMainPage(std::string& buffer) {
    std::regex findIndex ("lego/index.php");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findIndex)) {
      return true;
    }
    return false;
  }

  bool assembleFigure(std::string& buffer) {
    std::regex findAssemble ("assemble");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findAssemble)) {
      std::cout << "Assembling!!!" << std::endl;
      return true;
    }
    return false;
  }

  bool getImage(std::string& buffer) {
    std::regex findImage("lego/[^((index)||(list)||(assemble))]");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findImage)) {
      return true;
    }
    return false;
  }
};

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
        std::cout << figure << std::endl;
        std::string figureName = figure.substr(9, figure.size());
        std::cout << figureName << std::endl;
        if (this->tryConnection(figureName) != nullptr) {
          // write response

        // for the case where the figure does not exist
        } else {
          // erase the figure from the map

          // write response

        }

        responseString += "suuuuuuuuuup\n";

        // receive info from pieces server
        }
        break;

      case serverAction::requestingImage:
        this->requestImage(figure, responseString, responseVector);
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
      response += "404";
      return;
    }

    std::string figureBuffer = figure.substr(pos + 1, end - pos - 1);

    if (this->routingMap->count(figureBuffer) == 0) {
      response += "404";
      return;
    }

    std::unique_ptr<Socket> piecesServerConnection =
        std::move(tryConnection(figureBuffer)); // check for nullptr

    if (piecesServerConnection == nullptr) {
      response += "404";
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

  void reportAssembled (std::string& response, std::string figure) {

  }

  void reportNotAssemble (std::string& response, std::string figure) {

  }

  void getParts(std::string& response, std::string figure) {
    if (this->routingMap->count(figure) == 0) {
      response += "404";
      return;
    }

    std::unique_ptr<Socket> piecesServerConnection =
        std::move(tryConnection(figure)); // check for nullptr

    if (piecesServerConnection == nullptr) {
      response += "404";
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

// for handling the reponse to be sent to the client
class ResponseHandler : public Handler<std::shared_ptr<Response>>  { //Se encarga de las respuestas al cliente
 public:
  ResponseHandler(Queue<std::shared_ptr<Response>>* consumingQueue,
      std::shared_ptr<Response> stopCondition)
          : Handler(consumingQueue, stopCondition) {}

 private:
  void handleSingle(std::shared_ptr<Response> handlingData) {

    if (handlingData->requestType == serverAction::requestingImage) {
      if (handlingData->response == "404") {
        *handlingData->socket << handlingData->response;
        handlingData->socket->Close();
        return;
      }

      std::string response =
       // send header
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: image/jpg\r\n"
        "Content-Transfer-Enconding: binary\r\n"
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