#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"

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
  //1 sslread, operador sobre cargado.
    std::string buffer;

    std::cerr << "Entered handlers" << std::endl;

    while ((*handlingData >> buffer) == 500) {
      std::cout << buffer << std::endl;
    }

    std::cout << "exited handlers" << std::endl;

    std::shared_ptr<Request> request = std::make_shared<Request>(handlingData, "Chicki", serverAction::requestingParts);

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
    serverAction requestType = handlingData->requestType;
    std::string figure = handlingData->figure;

    std::cout << "Handling request and dummy connection to pieces server" << std::endl;
    std::cout << "Figure: " << figure << std::endl;

    if (handlingData == nullptr) {
      std::cout << "Error!!!!" << std::endl;
    }
    // create socket
    //Socket piecesServerConnection('s', false);

    //tryConnection(piecesServerConnection, figure);

    std::string responseReceived = "sup";

    // send info to pieces server
    switch(requestType) {
      case serverAction::requestingFigures:

        // receive info from pieces server

        break;
      case serverAction::requestingParts:

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

  bool tryConnection (
    Socket& piecesServerSocket,
    std::string figure) {
    std::string ip = (*this->routingMap)[figure].first;
    int port = (*this->routingMap)[figure].second;

    int tries = 1;
    // use information fetched to connect to pieces server
    while (tries <= 3 && !piecesServerSocket.Connect(ip.data(), port)) {
      // increase timeout

      tries++;
    }

    return tries != 3;
  }
};

// for handling the reponse to be sent to the client
class ResponseHandler : public Handler<std::shared_ptr<Response>>  { //Se encarga de las respuestas al cliente
 public:
  ResponseHandler(Queue<std::shared_ptr<Response>>* consumingQueue,
      std::shared_ptr<Response> stopCondition)
          : Handler(consumingQueue, stopCondition) {}

 private:
  void handleSingle(std::shared_ptr<Response> handlingData) { //HTTP Y HTML
    std::cout << "Final step before sending back to client!" << std::endl;

    std::string response =
        // send header
        "HTTP/1.1 200\r\n"
        "Content-type: text/html; charset=UTF-8\r\n"
        "Server: AttoServer v1.1\r\n"
        "\r\n"
        // send html format and title
        "<!DOCTYPE html>\n"
        "<html><body><h1>Conexión correcta!\n";

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

    std::cout << std::endl << "CODE: " << code
        << "\n\tip address: " << ip
        << "\n\tport:" << port << std::endl << std::endl;
  }

  void optionalToEnd () {
    std::cerr << "udp handler dying" << std::endl;
  }
};