#include <string>
#include <iostream>
#include <vector>

#include "Socket.hpp"

class Client {
 private: 
   std::vector<std::string> animalsArray;
   bool connected = false;
   Socket* socket;
   char type;
   bool IPv6;
 public:
   Client(char type, bool IPv6);
   int connectServer();
   bool makeRequest(std::string request);
   void Close();

 private:
   void processRequest(bool requestMenu);
   bool inAnimalArray(std::string animal);
   void regexAnalyzer(bool requestMenu, std::string& line);
   void mainMenuHandle();
   bool handleFigure();
   int Connect(const char * host, int port);
   int Write(const void *text, size_t size);
   int Read(void * text, size_t size);
    
};