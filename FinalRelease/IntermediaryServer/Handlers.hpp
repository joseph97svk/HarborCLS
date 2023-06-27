#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"


// for handling whatever the client sent
class ClientHandler : public Handler <Socket*> {
 private:

 public:
  ClientHandler(Queue<Socket*>* consumingQueue,
      Socket* stopCondition)
          : Handler(consumingQueue, stopCondition) {}

 private:
  // socket of client from which request is to be read
  void handleSingle(Socket* handlingData) {

    (void) handlingData;
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
  void handleSingle(Request* handlingData) {
    // use request to find ip and port from the map
    serverAction requestType = handlingData->requestType;
    std::string figure = handlingData->figure;

    // create socket
    Socket piecesServerConnection('s', false);

    std::string ip = (*this->routingMap)[figure].first;
    int port = (*this->routingMap)[figure].second;

    int tries = 1;
    // use information fetched to connect to pieces server
    while (tries <= 3 && !piecesServerConnection.Connect(ip.data(), port)) {
      // increase timeout

      tries++;
    }

    if (tries == 3) {

    }

    std::string responseReceived;

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
    piecesServerConnection.Close();

    //enqueue response
    std::shared_ptr<Response> response = std::make_shared<Response>(
        handlingData->socket,
        responseReceived,
        handlingData->requestType
        );

    this->responseQueue->push(response);
  }
};

// for handling the reponse to be sent to the client
class ResponseHandler : public Handler<std::shared_ptr<Response>>  {
 public:
  ResponseHandler(Queue<std::shared_ptr<Response>>* consumingQueue,
      std::shared_ptr<Response> stopCondition)
          : Handler(consumingQueue, stopCondition) {}

 private:
  void handleSingle(std::shared_ptr<Response> handlingData) {

    (void) handlingData;
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