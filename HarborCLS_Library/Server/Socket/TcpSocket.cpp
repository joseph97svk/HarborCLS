
//
// Created by josephvalverde on 12/11/23.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <expected>
#include <iostream>

#include "TcpSocket.hpp"
#include "Common/Error.hpp"

namespace HarborCLS {

  TcpSocket::TcpSocket(const bool ipv6) :
      _socketId(0),
      _ipv6(ipv6),
      _ssl(false),
      _sslController(std::nullopt) {
    this->setSocket();
  }

  TcpSocket::TcpSocket(const std::string &certFileName,
                       const std::string &keyFileName,
                       const bool ipv6 = false) :
      _socketId(0),
      _ipv6(ipv6),
      _ssl(true),
      _sslController(std::make_optional<SSLController>(certFileName, keyFileName)) {
    this->setSocket();
  }

  void TcpSocket::setSocket() {
    if (_ipv6) {
      _socketId = ::socket(AF_INET6, SOCK_STREAM, 0);
    } else {
      _socketId = ::socket(AF_INET, SOCK_STREAM, 0);
    }

    if (_socketId == -1) {
      throw std::runtime_error("TcpSocket::TcpSocket: Failed to create socket");
    }
  }

  TcpSocket::TcpSocket(const int socketFd) :
      _socketId(socketFd),
      _ipv6(false),
      _ssl(false) {
  }

  [[nodiscard]] std::expected<Success, SocketError> TcpSocket::connect(const std::string &host, const std::string &service) const {
    if (!_ipv6) {
      throw std::runtime_error("TcpSocket::connect: IPv4 not supported for connect by service");
    }
    if (_ssl) {
      std::expected<Success, SSLError> SSLConnection = _sslController->SSLConnect(_socketId);

      if (!SSLConnection) {
        return std::unexpected(SocketError("TcpSocket::connect: Failed to connect to host"
                                           , SocketErrors::GENERIC_ERROR));
      }
    }

    struct sockaddr_in6 host6 {};
    host6.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, host.c_str(), &host6.sin6_addr) == 1) {
      return std::unexpected(
          SocketError("TcpSocket::connect: Failed to convert host to IPv6 address"
                      , SocketErrors::GENERIC_ERROR));
    }

    struct addrinfo hints {}, *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    if (getaddrinfo(host.c_str(), service.c_str(), &hints, &result) != 0) {
      freeaddrinfo(result);
      return std::unexpected(
          SocketError("TcpSocket::connect: Failed to get address info"
                      , SocketErrors::GENERIC_ERROR));

    }

    for (rp = result; rp; rp = rp->ai_next) {
      if (::connect(_socketId, rp->ai_addr, rp->ai_addrlen) == 0)
        break;
    }

    freeaddrinfo(result);

    return Success();
  }

  [[nodiscard]] std::expected<Success, SocketError> TcpSocket::connect(const std::string &host, const int targetPort) const {
    std::expected<Success, Error<SocketErrors>> connectionResult =
        _ipv6 ? this->ipv6Connect(host, targetPort) : this->ipv4Connect(host, targetPort);

    if (!connectionResult) {
      return connectionResult;
    }

    if (_ssl) {
      std::expected<Success, SSLError> sslResult = _sslController->SSLConnect(_socketId);

      if (!sslResult) {
        return std::unexpected(SocketError("TcpSocket::connect: Failed to connect to host"
                                           , SocketErrors::GENERIC_ERROR));
      }
    }

    return Success();
  }

  [[nodiscard]] std::expected<Success, SocketError> TcpSocket::ipv4Connect(const std::string &host, int port) const {
    struct sockaddr_in host4 {};
    host4.sin_family = AF_INET;
    host4.sin_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &host4.sin_addr) == -1) {
      return std::unexpected(SocketError("TcpSocket::connect: Failed to convert host to IPv4 address"
                                         , SocketErrors::GENERIC_ERROR));
    }

    switch (::connect(_socketId, reinterpret_cast<sockaddr*>(&host4), sizeof(host4))) {
      case 0:
        return Success();
      case ECONNREFUSED:
        return std::unexpected(SocketError("TcpSocket::connect: Connection refused"
                                           , SocketErrors::SOCKET_CONNECTION_REFUSED));
      default:
        break;
    }

    return std::unexpected(SocketError("TcpSocket::connect: Failed to connect to host"
        , SocketErrors::GENERIC_ERROR));
  }

  [[nodiscard]] std::expected<Success, SocketError> TcpSocket::ipv6Connect(const std::string &host, int port) const {
    struct sockaddr_in6 host6 {};
    host6.sin6_family = AF_INET6;
    host6.sin6_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET6, host.c_str(), &host6.sin6_addr) == -1) {
      return std::unexpected(SocketError("TcpSocket::connect: Failed to convert host to IPv6 address"
                                         , SocketErrors::GENERIC_ERROR));
    }

    switch (::connect(_socketId, reinterpret_cast<sockaddr*>(&host6), sizeof(host6))) {
      case 0:
        return Success();
      case ECONNREFUSED:
        return std::unexpected(SocketError("TcpSocket::connect: Connection refused"
            , SocketErrors::SOCKET_CONNECTION_REFUSED));
      default:
        break;
    }

    return std::unexpected(SocketError("TcpSocket::connect: Failed to connect to host"
        , SocketErrors::GENERIC_ERROR));
  }

  void TcpSocket::close() const {
    ::close(_socketId);
  }

  template<typename readDatatype>
  TcpSocket &TcpSocket::operator>>(readDatatype &data) {
    std::vector<char> buffer;
    std::pair<std::vector<char>, unsigned int> readData;

    std::function<std::pair<std::vector<char>, unsigned int>()> readOperation;

    if (_ssl) {
      readOperation =
          [capture = &_sslController.value()] {
            return capture->SSLRead();
          };
    } else {
      readOperation = [this] {
        return Read();
      };
    }

    do {
      readData = readOperation();
      buffer.insert(buffer.end(), readData.first.begin(), readData.first.end());
      std::cout << "<<<< Socket has read: " << readData.second << " bytes, "
                                                                  " buffer size is:"
                                                                  << BUFFER_SIZE << std::endl;
    } while (readData.second == BUFFER_SIZE);

    data = readDatatype(buffer.begin(), buffer.end());

    return *this;
  }

  template<typename writeDatatype>
  TcpSocket &TcpSocket::operator<<(const writeDatatype &data) {
    std::span<char> dataSpan = std::span<char>(data.data(), data.size());

    unsigned int bytesWritten = _ssl ?
                                _sslController->SSLWrite(dataSpan) :
                                this->Write(dataSpan);

    if (bytesWritten != data.size()) {
      throw std::runtime_error("TcpSocket::operator<<: Failed to write all bytes to socket");
    }

    return *this;
  }

  TcpSocket &TcpSocket::operator<<(std::vector<char> &data) {
    size_t bytesWritten = _ssl ?
                                _sslController->SSLWrite(std::span<char>(data.data(), data.size())) :
                                this->Write(std::span<char>(data.data(), data.size()));

    if (bytesWritten != data.size()) {
      throw std::runtime_error("TcpSocket::operator<<: Failed to write all bytes to socket");
    }
    return *this;
  }

  TcpSocket &TcpSocket::operator>>(std::vector<char> &data) {
    std::pair<std::vector<char>, long long> readData;
    std::function<std::pair<std::vector<char>, unsigned int>()> readOperation;

    if (_ssl) {
      readOperation =
          [capture = &_sslController.value()] {
            return capture->SSLRead();
          };
    } else {
      readOperation = [this] {
        return Read();
      };
    }

    do {
      std::cout << "<<<< Socket is reading >>>>" << std::endl;
      try {
        readData = readOperation();
      } catch (const std::exception &e) {
        std::cout << "en exception was encountered while reading! : " << e.what() << std::endl;
        return *this;
      }

      std::cout << "<<<< Socket has read: "
          << readData.second << " bytes, "
          " buffer size is:"
          << BUFFER_SIZE  << " >>>>" << std::endl;

      data.insert(data.end(), readData.first.begin(), readData.first.end());
    } while (readData.second == BUFFER_SIZE);

    return *this;
  }

  void TcpSocket::bind(const unsigned int portToBindTo) const {
    uint16_t port = htons(static_cast<uint16_t>(portToBindTo));
    struct sockaddr_in host4 {
        .sin_family = AF_INET,
        .sin_port = port,
        .sin_addr = { INADDR_ANY },
        .sin_zero = { 0 }
    };
    struct sockaddr_in6 host6 {
        .sin6_family = AF_INET6,
        .sin6_port = port,
        .sin6_flowinfo = 0,
        .sin6_addr = in6addr_any,
        .sin6_scope_id = 0
    };

    std::pair<sockaddr *, socklen_t> ha = _ipv6 ?
        std::make_pair(reinterpret_cast<sockaddr *>(&host6), sizeof(host6)) :
        std::make_pair(reinterpret_cast<sockaddr *>(&host4), sizeof(host4));

    if (::bind(_socketId, ha.first, ha.second) == -1) {
      throw std::runtime_error("TcpSocket::bind: Failed to bind to port: " + std::to_string(portToBindTo));
    }
  }

  void TcpSocket::listen(const unsigned int queueSize) const {
    if (::listen(_socketId, static_cast<int>(queueSize)) == -1) {
      throw std::runtime_error("TcpSocket::listen: Failed to listen on socket");
    }
  }

  [[nodiscard]] std::expected<std::shared_ptr<TcpSocket>, SocketError>TcpSocket::accept() const {
    struct sockaddr_in server {};
    socklen_t addr_len = sizeof(server);

    int socketFD =
        ::accept(_socketId,
                 reinterpret_cast<sockaddr *>(&server),
                 &addr_len
        );

    if (socketFD == -1) {
      return std::unexpected(SocketError("TcpSocket::accept: Failed to accept connection"
                                         , SocketErrors::GENERIC_ERROR));
    }

    std::shared_ptr<TcpSocket> acceptedConnection = std::make_shared<TcpSocket>(socketFD);

    if (_ssl) {
      acceptedConnection->_sslController->SSLCreate(*_sslController, _socketId);
      acceptedConnection->_sslController->SSLAccept();
    }

    return acceptedConnection;
  }

  [[nodiscard]] bool TcpSocket::isSSL() const noexcept {
    return _ssl;
  }

  inline size_t TcpSocket::Write(const std::span<char> &message) const {
    size_t bytesWritten = ::write(_socketId, message.data(), message.size());

    if (bytesWritten != message.size()) {
      throw std::runtime_error("TcpSocket::Write: Failed to write all bytes to socket");
    }

    return bytesWritten;
  }

  inline std::pair<std::vector<char>, unsigned int> TcpSocket::Read() const {
    std::vector<char> buffer(BUFFER_SIZE);
    std::cout << "---> read operation" << std::endl;
    long long bytesRead = ::read(_socketId, buffer.data(), buffer.size());
    std::cout << "---> read operation finished" << std::endl;
    if (bytesRead < 0) {
      return std::make_pair(std::vector<char>(), -1);
    }

    buffer.resize(bytesRead);

    return std::make_pair(buffer, bytesRead);
  }

  void TcpSocket::setTimeout(const size_t seconds, const size_t microseconds) const {
    timeval timeout {
        .tv_sec = static_cast<time_t>(seconds),
        .tv_usec = static_cast<suseconds_t>(microseconds)
    };

    if (setsockopt(_socketId, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof timeout) < 0) {
      throw std::runtime_error("TcpSocket::setTimeout: Failed to set timeout");
    }
    if (setsockopt(_socketId, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                   sizeof timeout) < 0) {
      throw std::runtime_error("TcpSocket::setTimeout: Failed to set timeout");
    }
  }

  [[nodiscard]] bool TcpSocket::isIpV6() const noexcept {
    return _ipv6;
  }

  std::expected<Success, Error<SocketErrors>> TcpSocket::sendDecoyConnection(std::string &ip, int port) const {
    return this->connect(ip, port);
  }
}