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

    std::cerr << "Entered handlers  kagsdkahkdhakhsdkahskdhkashdkahskdha" << std::endl;

    while ((*handlingData >> buffer) == 500) {
      std::cout << buffer << std::endl;
    }

    std::cout << "exited handlers lahsdkajhksdhakdkadhkakhsdhjkajhkdahksd" << std::endl;


    std::shared_ptr<Request> request = std::make_shared<Request>(handlingData, "Chicki", serverAction::requestingParts);

    this->requestQueue->push(request);
  }

  void optionalToEnd () {
    this->requestQueue->push(nullptr);
    std::cerr << "Client handler dying" << std::endl;
  }
};




// Clase RequestHandler para manejar las solicitudes recibidas
class RequestHandler : public Handler<std::shared_ptr<Request>>  {
  RoutingMap* routingMap;
  Queue<std::shared_ptr<Response>>* responseQueue;
 public:
  // Constructor de RequestHandler
  // Toma una cola de solicitudes para consumir, una solicitud de parada, un mapa de enrutamiento y una cola de respuestas
  RequestHandler(Queue<std::shared_ptr<Request>>* consumingQueue,
      std::shared_ptr<Request> stopCondition, 
      RoutingMap* routingMap, 
      Queue<std::shared_ptr<Response>>* responseQueue)
          : Handler(consumingQueue, stopCondition)
          , routingMap(routingMap)
          , responseQueue(responseQueue){}

 private:
  // Método para manejar una sola solicitud
  void handleSingle(std::shared_ptr<Request> handlingData) {
    // Utilizar la solicitud para obtener la acción del servidor y la figura asociada
    serverAction requestType = handlingData->requestType;
    std::string figure = handlingData->figure;

    std::cout << "Handling request and dummy connection to pieces server" << std::endl;
    std::cout << "Figure: " << figure << std::endl;

    if (handlingData == nullptr) {
      std::cout << "Error!!!!" << std::endl;
    }

    // Crear un socket (Socket piecesServerConnection) para conectarse al servidor de piezas
    // y enviar la solicitud y recibir la respuesta
    // NOTA: El código para la conexión al servidor de piezas está comentado y se necesita implementar

    // Enviar información al servidor de piezas según el tipo de solicitud (requestType)
    switch(requestType) {
      case serverAction::requestingFigures:
        // Enviar solicitud de figuras al servidor de piezas

        // Recibir información del servidor de piezas

        break;
      case serverAction::requestingParts:
        // Enviar solicitud de partes al servidor de piezas

        // Recibir información del servidor de piezas

        break;
      case serverAction::requestingAssembly:
        // Enviar solicitud de ensamblaje al servidor de piezas

        // Recibir información del servidor de piezas

        break;
      default:
        break;
    }

    // Cerrar el socket de conexión con el servidor de piezas
    // NOTA: El código para cerrar el socket está comentado y se necesita implementar

    // Crear un objeto de respuesta compartido y asignarle los valores necesarios
    std::shared_ptr<Response> response = std::make_shared<Response>(
        handlingData->socket,
        "responseReceived asdhaksdhkahsdkhaksdh", // Aquí debe ir la respuesta recibida del servidor de piezas
        handlingData->requestType
    );

    // Agregar la respuesta a la cola de respuestas
    this->responseQueue->push(response);
  }

  // Método opcional para finalizar el manejo de solicitudes
  void optionalToEnd() {
    std::cerr << "request handler dying" << std::endl;
    this->responseQueue->push(nullptr);
  }

  // Método para intentar establecer la conexión con el servidor de piezas
  std::unique_ptr<Socket> tryConnection (std::string figure) {
    std::unique_ptr<Socket> piecesServerSocket = std::make_unique<Socket>('s', false);

    std::string ip = (*this->routingMap)[figure].first;
    int port = (*this->routingMap)[figure].second;

    // Establecer un tiempo de espera (timeout) para la conexión
    piecesServerSocket->increaseTimeout(FIRST_TIMEOUT);

    // Intento de conexión 1
    if(piecesServerSocket->Connect(ip.data(), port)) {
      return piecesServerSocket;
    }

    // Establecer un tiempo de espera (timeout) más largo
    piecesServerSocket->increaseTimeout(SECOND_TIMEOUT);

    // Intento de conexión 2
    if(piecesServerSocket->Connect(ip.data(), port)) {
      return piecesServerSocket;
    }

    // Establecer un tiempo de espera (timeout) aún más largo
    piecesServerSocket->increaseTimeout(THIRD_TIMEOUT);

    // Intento de conexión 3
    if(piecesServerSocket->Connect(ip.data(), port)) {
      return piecesServerSocket;
    }

    // Si ninguno de los intentos anteriores tiene éxito, se devuelve un puntero nulo para indicar el fallo en la conexión
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
        "HTTP/1.1 200\r\n"
        "Content-type: text/html; charset=UTF-8\r\n"
        "Server: AttoServer v1.1\r\n"
        "\r\n"
        // send html format and title
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "<style>\n"
        "body {\n"
        "  text-align: center;\n"
        "  font-family: Arial, sans-serif;\n"
        "  font-size: 32px;\n"  // Tamaño de fuente original duplicado (16px * 2 = 32px)
        "  background-color: beige;\n"  // Fondo beige agregado
        "}\n"
        "#title {\n"
        "  font-size: 64px;\n"  // Tamaño de fuente más grande duplicado (32px * 2 = 64px)
        "  animation: colorChange 0.5s infinite linear;\n"  // Cambio de color más rápido (0.5s)
        "}\n"
        "strong {\n"
        "  font-weight: bold;\n"  // Negrita agregada
        "}\n"
        "select[name=\"figures\"] {\n"
        "  font-size: 48px;\n"  // Tamaño de fuente un poco más pequeño (32px * 1.5 = 48px)
        "}\n"
        "@keyframes colorChange {\n"
        "  0% { color: red; }\n"
        "  14% { color: orange; }\n"
        "  28% { color: yellow; }\n"
        "  42% { color: green; }\n"
        "  57% { color: blue; }\n"
        "  71% { color: indigo; }\n"
        "  85% { color: violet; }\n"
        "  100% { color: red; }\n"
        "}\n"
        "</style>\n"
        "<title>ESJOJO</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1 id=\"title\">ESJOJO</h1>\n"
        "<p><strong>¡Bienvenido al servidor intermedio!</strong></p>\n" // Texto añadido en negrita
        "<p><strong>Escoge una figura</strong></p>\n" // Texto añadido en negrita
        "<div class=\"st12\">\n"
        "   <form>\n"
        "      <select name=\"figures\" onchange=\"goFigure(this.form)\">\n"
        "         <option value=\"none\"> Elegir </option>\n"
        "         <option value=\"horse\"> Caballo </option>\n"
        "         <option value=\"elephant\"> Elefante </option>\n"
        "         <option value=\"giraffe\"> Jirafa </option>\n"
        "         <option value=\"whitesheep\"> Oveja blanca </option>\n"
       "         <option value=\"blacksheep\"> Oveja negra </option>\n"
        "         <option value=\"duck\"> Pato </option>\n"
        "         <option value=\"fish\"> Pez </option>\n"
        "         <option value=\"chiki\"> Chiki </option>\n" // Agregada la opción "chiki"
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

    // insert figures
    this->insertFigures(buffer, ip, port);
  
    std::string chiki = "Chicki";

    std::cout << "{<" << (*this->routingMap)[chiki].first << ", "
        << (*this->routingMap)[chiki].second << ">}" << std::endl;
  }

  void insertFigures(std::string& buffer, const std::string& ip, const int port) {
    std::string figure;

    size_t nextFigure = 0,
        end = 0;

    // remove everything before figure
    nextFigure = buffer.find(29) + 1;

    buffer = buffer.substr(nextFigure, buffer.size());
    do {
      // find next divisor
      nextFigure = buffer.find(29) + 1;

      // if no figure
      if (nextFigure == std::string::npos) {
        // exit and do nothing
        break;
      }

      // check for end of figure
      end = buffer.find(29);

      // get figure
      std::string figure = buffer.substr(nextFigure, end);

      // insert the figure and the info 
      this->routingMap->insert({figure, {ip, port}});

      // remove figure
      buffer = buffer.substr(nextFigure, buffer.size());

      if (end == std::string::npos) {
        end = buffer.size();
      }

    // as long as the end is not the end of the everything
    } while (end != buffer.size());
  }

  void optionalToEnd () {
    std::cerr << "udp handler dying" << std::endl;
  }
};