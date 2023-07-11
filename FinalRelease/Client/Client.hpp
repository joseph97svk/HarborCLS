#include <string>
#include <iostream>
#include <vector>
#include <utility>

#include "Socket.hpp"

enum RequestType {
  Base, 
  MenuRequest,
  FigureRequest,
  Server,
  Exit
};

class Client {
 private: 
  std::vector<std::string> figuresArray;
  bool connected = false;
  char type;
  bool IPv6;
  Socket* socket;
  std::string currentFigure = "";
  std::vector<std::pair<std::string, size_t>> requestedPieces;
  size_t port;
  char* host;

 public:
  Client(char type, bool IPv6, size_t port, char* host);
  int connectServer();
  int run(RequestType);
  bool makeRequest(RequestType);
  void Close();
  void showPiecesServer();

 private:
  void processRequest(RequestType requestMenu);
  bool inFigureArray(std::string figure);
  bool inRequestedPieces(std::string& piece);
  void regexAnalyzer(RequestType requestMenu, std::string& line, int& totalAmount);
  RequestType mainMenuHandle();
  RequestType handleFigure();
  RequestType handleServerRequest();
  int Connect(const char * host, int port);
  int Connect(const char * host, const char * service);
  int Write(const void *text, size_t size);
  int Read(void * text, size_t size);
    
};