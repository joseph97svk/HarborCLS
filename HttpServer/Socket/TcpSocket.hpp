//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_TCPSOCKET_HPP
#define HTTPSERVER_TCPSOCKET_HPP

#include <memory>
#include <span>
#include <optional>
#include "SSL/SSLController.hpp"

#include "common.hpp"

class TcpSocket {
  int socketId;
  int port;
  bool ipv6;
  bool ssl;

  std::optional<SSLController> sslController;

  const unsigned int BUFFER_SIZE = 512;
public:
  explicit TcpSocket(int port, bool ipv6);
  TcpSocket(int port, const std::string& certFileName, const std::string& keyFileName, bool ipv6);
  explicit TcpSocket(int socketFd);

  //NO_COPY(TcpSocket)

  ~TcpSocket() = default;

  void connect(const std::string& host, int targetPort);
  void connect(const std::string& host, const std::string& service) const;

  void close() const;

  template<typename readDatatype>
  TcpSocket& operator>>(readDatatype& data);

  template<typename writeDatatype>
  TcpSocket& operator<<(const writeDatatype& data);

  TcpSocket& operator<<(std::vector<char>& data);
  TcpSocket& operator>>(std::vector<char>& data);

  void bind(unsigned int portToBindTo) const;
  void listen(unsigned int queueSize) const;
  [[nodiscard]] std::shared_ptr<TcpSocket> accept() const;

  [[nodiscard]] bool isSSL() const;
  void setTimeout(size_t seconds, size_t microseconds) const;

  char getType() {
    return 's';
  }

private:
  void setSocket();

  void ipv4Connect(const std::string& host, int port) const;
  void ipv6Connect(const std::string& host, int port) const;

  [[nodiscard]] unsigned int Write(const std::span<char>& message) const;
  [[nodiscard]] std::pair<std::vector<char>, unsigned int> Read() const;
};

#endif //HTTPSERVER_TCPSOCKET_HPP