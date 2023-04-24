#include <string>
#include <iostream>
#include <vector>

#include "Socket.hpp"

class Client {
 private: 
  std::vector<std::string> figuresArray;
  bool connected = false;
  char type;
  bool IPv6;
  Socket* socket;
  std::string currentFigure = "";

  enum RequestType {
  MenuRequest,
  FigureRequest
};

   
 public:
  Client(char type, bool IPv6);
  int connectServer();
  bool makeRequest(std::string request, RequestType);
  void Close();

 private:
  void processRequest(RequestType requestMenu);
  bool inFigureArray(std::string figure);
  void regexAnalyzer(RequestType requestMenu, std::string& line, int& totalAmount);
  int mainMenuHandle();
  int handleFigure();
  int Connect(const char * host, int port);
  int Connect(const char * host, const char * service);
  int Write(const void *text, size_t size);
  int Read(void * text, size_t size);
    
};