#ifndef CLIENTHANDLER_HPP
#define CLIENTHANDLER_HPP

#include "Generics/Handler.hpp"
#include "Generics/RoutingMap.hpp"

#include <regex>
#include <fstream>
#include <algorithm>

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

    if (buffer.size() == 0) {
      std::cout << "Empty request :: no HTTP or other format identified" << std::endl;
      return;
    }

    std::regex findHttp("(.* )HTTP/1.1");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    std::string httpRequest;

    if (std::regex_search(begin, buffer.cend(), requestMatch, findHttp)) {
      httpRequest = requestMatch[1];
      std::cout << "\tRequest: " << httpRequest << "\n\t:=  "; 
    } else {
      std::cout << "\tInvalid request :: no HTTP or other format identified" << std::endl;
      return;
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

    if (iconRequest(httpRequest)) {
      action = serverAction::requestingBrowserIcon;
    }

    std::cout << "action: " << action << std::endl << std::endl;

    std::shared_ptr<Request> request = std::make_shared<Request>(handlingData, httpRequest, action);

    this->requestQueue->push(request);
  }

  void optionalToEnd () {
    this->requestQueue->push(nullptr);
    std::cerr << "Client handler dying" << std::endl;
  }

  inline bool iconRequest(std::string& buffer) {
    std::regex findIcon("favicon.ico");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findIcon)) {
      return true;
    }
    return false;
  }

  inline bool isMainPage(std::string& buffer) {
    std::regex findIndex ("lego/index.php");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findIndex)) {
      return true;
    }
    return false;
  }

  inline bool assembleFigure(std::string& buffer) {
    std::regex findAssemble ("assemble");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findAssemble)) {
      return true;
    }
    return false;
  }

  inline bool getImage(std::string& buffer) {
    std::regex findImage("(.jpg)|(.png)|(.jpeg)");

    std::smatch requestMatch;

    std::string::const_iterator begin(buffer.cbegin());

    if (std::regex_search(begin, buffer.cend(), requestMatch, findImage)) {
      return true;
    }
    return false;
  }
};

#endif