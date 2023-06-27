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

    // create socket

    // use information fetched to connect to pieces server

    // send info to pieces server

    // receive info from pieces server

    // close socket

    //enqueue response
    (void) handlingData;
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