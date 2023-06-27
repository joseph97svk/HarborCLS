#ifndef COMMON_HPP
#define COMMON_HPP

#include <memory>
#include <string>

#include "Socket/Socket.hpp"


struct Request {
  std::shared_ptr<Socket> socket;
  std::string figure;

  Request(Socket* socket, std::string figure)
      : socket()
      , figure(figure) {
        this->socket.reset(socket);
      }
};

struct Response {
  std::shared_ptr<Socket> socket;
  std::string response;

  Response(Socket* socket, std::string response)
      : socket()
      , response(response) {
        this->socket.reset(socket);
      }
};

#endif