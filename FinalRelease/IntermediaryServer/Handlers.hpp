#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"

#include <regex>

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
  void handleSingle(std::shared_ptr<Socket> handlingData) {
  //1 sslread, operador sobre cargado.
    std::string buffer;

    std::cerr << "Entered handlers" << std::endl;

    while ((*handlingData >> buffer) == 500) {
      std::cout << buffer << std::endl;
    }

    std::cout<<"CONTENIDO DESPUES DEL WHILE "<<buffer<<std::endl;
    std::cout<<"ESTO ES TODO "<<std::endl;

    std::regex findHttp("((.* )HTTP/1.1)");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    std::string httpRequest;

    if (std::regex_search(begin, buffer.cend(), requestMatch, findHttp)) {
      std::cout << "found something" << std::endl;
      httpRequest = requestMatch[0];
      std::cout << "request: " << httpRequest << std::endl;
      httpRequest = requestMatch[1];
      std::cout << "request: ," << httpRequest << "," << std::endl;
      httpRequest = requestMatch[2];
      std::cout << "request: " << httpRequest << std::endl;
    }
// http://localhost:2020

    // default is asking for figures
    serverAction action = serverAction::requestingFigures;

    // if asking for icon
    if (httpRequest == "favicon.ico" || httpRequest == "favicon.ico ") {
      action = serverAction::requestingBrowserIcon;
    // for all else, as long as not empty request
    } else if (httpRequest != " ") {
      action = serverAction::requestingFigures;
      size_t figureStartingPos = httpRequest.find('=') + 1;
      httpRequest = httpRequest.substr(figureStartingPos, httpRequest.size());
    }

    std::cout << "Figure: " << httpRequest << std::endl;
    
    // http://localhost:2020/lego/list.php?figure=chiki

    std::cout << "exited handlers" << std::endl;

    std::shared_ptr<Request> request = std::make_shared<Request>(handlingData, httpRequest, action);

    this->requestQueue->push(request);
  }

  void optionalToEnd () {
    this->requestQueue->push(nullptr);
    std::cerr << "Client handler dying" << std::endl;
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
    //std::cout<<"ALMACENAMIENTO DENTRO DE handlingData" << handlingData->figure<< std::endl;
    serverAction requestType = handlingData->requestType;
    std::string figure = handlingData->figure;

    std::cout << "Handling request and dummy connection to pieces server" << std::endl;
    std::cout << "RequestHandler :: Figure: " << figure << std::endl;

    if (handlingData == nullptr) {
      std::cout << "Error!!!!" << std::endl;
    }

    // create socket
    //Socket piecesServerConnection = std::move(tryConnection(figure)); // check for nullptr

    //tryConnection(piecesServerConnection, figure);

    std::string responseReceived = " ";

    // send info to pieces server
    switch(requestType) {
      case serverAction::requestingFigures:
        for (auto figure : *(this->routingMap)) {
          responseReceived += figure.first;

        }
        // receive info from pieces server

        break;
      case serverAction::requestingParts:
      responseReceived = requestingParts;
        // receive info from pieces server

        break;
      case serverAction::requestingAssembly:
        
        // receive info from pieces server
        break;
      default:
        break;
    }

    // close socket
    //piecesServerConnection.Close();

    //enqueue response
    std::shared_ptr<Response> response = std::make_shared<Response>(
        handlingData->socket,
        responseReceived,
        handlingData->requestType
        );

    this->responseQueue->push(response);
  }

  void optionalToEnd () {
    std::cerr << "request handler dying" << std::endl;
    this->responseQueue->push(nullptr);
  }

  std::unique_ptr<Socket> tryConnection (std::string figure) {
    std::unique_ptr<Socket> piecesServerSocket = std::make_unique<Socket>('s', false);

    std::string ip = (*this->routingMap)[figure].first;
    int port = (*this->routingMap)[figure].second;

    // set timeout
    piecesServerSocket->increaseTimeout(FIRST_TIMEOUT);
    // first try
    if(piecesServerSocket->Connect(ip.data(), port)) {
      return piecesServerSocket;
    }

    // increase timeout
    piecesServerSocket->increaseTimeout(SECOND_TIMEOUT);
    // second try
    if(piecesServerSocket->Connect(ip.data(), port)) {
      return piecesServerSocket;
    }

    // increase timeout
    piecesServerSocket->increaseTimeout(THIRD_TIMEOUT);
    // final try
    if(piecesServerSocket->Connect(ip.data(), port)) {
      return piecesServerSocket;
    }

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
    std::cout << "Final step before sending back to client!" << std::endl;

  std::string response =
    // send header
    "HTTP/1.1 200 OK\r\n"
    "Content-type: text/html; charset=UTF-8\r\n"
    "Server: AttoServer v1.1\r\n"
    "\r\n"
    // send html format and title
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<script>\n"
    "function goFigure(form) {\n"
    "   figure = form.figures.options[form.figures.selectedIndex].value;\n"
    "   form.figures.selectedIndex = 0;\n"
    "   if (figure === 'caballo') {\n"
    "       location.href = '/lego/horse.php';\n"
    "   } else {\n"
    "       location.href = '/lego/list.php?figure=' + figure;\n"
    "   }\n"
    "}\n"
    "</script>\n"
    "<style>\n"
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
    "   <h1>¡Bienvenido al servidor intermedio!</h1>\n"
    "   <form>\n"
    "      <select name=\"figures\" onchange=\"goFigure(this.form)\">\n"
    "         <option value=\"None\"> Elegir </option>\n"
    "         <option value=\"caballo\"> Caballo </option>\n"
    "         <option value=\"elefante\"> Elefante </option>\n"
    "         <option value=\"jirafa\"> Jirafa </option>\n"
    "         <option value=\"ovejablanca\"> Oveja blanca </option>\n"
    "         <option value=\"ovejanegra\"> Oveja negra </option>\n"
    "         <option value=\"pato\"> Pato </option>\n"
    "         <option value=\"pez\"> Pez </option>\n"
    "         <option value=\"chiki\"> Chiki </option>\n"
    "      </select>\n"
    "   </form>\n"
    "</div>\n"
    "</body>\n"
    "</html>\n";

    response += handlingData->response;
    response += "</h1></body></html>";

    *handlingData->socket << response;

    std::cout << "Request handling completed" << std::endl;
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
    
    std::cout << ip << std::endl;

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