#ifndef COMMON_HPP
#define COMMON_HPP

#include <memory>
#include <string>
#include <netdb.h>
#include <filesystem>

#include "Socket/Socket.hpp"

#define NO_COPY(class) \
  class(const class&) = delete; \
  class& operator=(const class&) = delete; \
  class(class&&) = delete; \
  class& operator=(class&&) = delete;

enum serverAction {
  requestingFigures,
  requestingParts,
  requestingAssembly,
  piecesServerError,
  requestingBrowserIcon,
  requestingImage,
  inMapNotFound
};

struct Request {
  std::shared_ptr<Socket> socket;
  std::string figure;
  serverAction requestType;

  Request(std::shared_ptr<Socket> socket, std::string figure, serverAction requestType)
      : socket(socket)
      , figure(std::move(figure))
      , requestType(requestType) {
      }
};

struct Response {
  std::shared_ptr<Socket> socket;
  std::string response;
  serverAction requestType;
  std::vector<char> responseVec;

  // Response(std::shared_ptr<Socket> socket, std::string response, serverAction requestType)
  //     : socket()
  //     , response(response)
  //     , requestType(requestType) {
  //   this->socket.reset(socket);
  // }

  Response(std::shared_ptr<Socket> socket, std::string& response, serverAction requestType, std::vector<char>& responseVector)
      : socket(socket)
      , response(std::move(response))
      , requestType(requestType)
      , responseVec(std::move(responseVector)) {
  }
};

[[nodiscard]] inline static std::string getComputerIp () {
  std::string host;
  host.resize(256);

  gethostname(host.data(), sizeof(host));

  struct hostent* hostentry = gethostbyname(host.data());

  host.clear();
  host = inet_ntoa(*((struct in_addr*) hostentry->h_addr_list[0]));

  return host;
}

[[nodiscard]] inline static std::string getCurrentTimeAndDate() {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_c), "%F.%T");

  return ss.str();
}

[[nodiscard]] inline static std::string getCurrentTime() {
  auto now = std::chrono::system_clock::now();
  auto now_c = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&now_c), "%T");

  return ss.str();
}

#endif
