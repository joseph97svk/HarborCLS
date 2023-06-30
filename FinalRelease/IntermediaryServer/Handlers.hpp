#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"

// for handling whatever the client sent
class ClientHandler : public Handler <Socket*> {
 private:
  Queue<Request*>* requestQueue;

 public:
  ClientHandler(Queue<Socket*>* consumingQueue,
      Queue<Request*>* requestQueue,
      Socket* stopCondition)
          : Handler(consumingQueue, stopCondition)
          , requestQueue (requestQueue){}

 private:
  // socket of client from which request is to be read
  void handleSingle(Socket* handlingData) { //Esta chuncha ya está conectada
  //1 sslread, operador sobre cargado.
    std::string buffer;

    std::cout << "Entered handlers" << std::endl;
    int bytesRead = 500;
    while ((*handlingData >> buffer) == 500) {
      std::cout << buffer << std::endl;
    }

    std::cout << "exited handlers" << std::endl;

    Request* request = new Request(handlingData, "Chicki", serverAction::requestingParts);

    this->requestQueue->push(request);
  }
};

class RequestHandler : public Handler<Request*>  {
  RoutingMap* routingMap;
  Queue<std::shared_ptr<Response>>* responseQueue;
 public:
  RequestHandler(Queue<Request*>* consumingQueue,
      Request* stopCondition, 
      RoutingMap* routingMap, 
      Queue<std::shared_ptr<Response>>* responseQueue)
          : Handler(consumingQueue, stopCondition)
          , routingMap(routingMap)
          , responseQueue(responseQueue){}

 private:
  void handleSingle(Request* handlingData) { //HTTP
    // use request to find ip and port from the map
    serverAction requestType = handlingData->requestType;
    std::string figure = handlingData->figure;

    std::cout << "Handling request and dummy connection to pieces server" << std::endl;
    std::cout << "Figure: " << figure << std::endl;
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

      if (tries == 3) {

    }
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
  }
};

// handle all broadcasts received
class UDPHandler : public Handler<Socket*> {
 private:
  RoutingMap* routingMap;
 public:
  UDPHandler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition, RoutingMap* routingMap)
          : Handler(consumingQueue, stopCondition)
          , routingMap(routingMap){}

 private:
  // socket of client to send response
  void handleSingle(Socket* handlingData) {
    (void) handlingData;
  }
};