//
// Created by josephvalverde on 12/12/23.
//

#include <stdexcept>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include <unistd.h>

#include "UdpSocket.hpp"

namespace HarborCLS {

  UDPSocket::UDPSocket(bool ipv6)
    : _ipv6(ipv6) {

    if (_ipv6) {
      this->_socketId = ::socket(AF_INET6, SOCK_DGRAM, 0);
    } else {
      this->_socketId = ::socket(AF_INET, SOCK_DGRAM, 0);
    }

    if (_socketId < 0) {
      throw std::runtime_error("Error creating socket");
    }
  }

  UDPSocket::UdpStream UDPSocket::operator[](std::string ip, int port, size_t readBufferSize) {
    bool broadcast = false;
    if (!ip.empty()) {
      broadcast = ip[0] == '-';
    }

    if (ip.empty()) {
      broadcast = true;
    }

    if (broadcast && !ip.empty()) {
      ip = ip.substr(1, ip.size());
    }

    return {*this, ip, port, broadcast, readBufferSize};
  }

  void UDPSocket::setBroadcast(const bool broadcast) const {
    int broadcastEnable = static_cast<int>(broadcast);

    if (setsockopt(
        this->_socketId,
        SOL_SOCKET,
        SO_BROADCAST,
        &broadcastEnable,
        sizeof(broadcastEnable)) < 0) {
      throw std::runtime_error("Error setting broadcast");
    }
  }

  inline void UDPSocket::sendTo(const std::span<char> &data, size_t size, const sockaddr_in &sockInfo) const {
    if (sendto(
        this->_socketId,
        data.data(),
        size,
        0,
        reinterpret_cast<const sockaddr *>(&sockInfo),
        sizeof(sockInfo)) <= 0) {
      throw std::runtime_error("Error sending data");
    }
  }

  inline std::vector<char> UDPSocket::recvFrom(size_t size, sockaddr_in &sockInfo) const {
    std::vector<char> data(size);

    socklen_t sockInfoSize = sizeof(sockaddr);

    size_t bytesRead = recvfrom(
        this->_socketId,
        data.data(),
        size,
        0,
        reinterpret_cast<sockaddr *>(&sockInfo),
        &sockInfoSize);

    if (bytesRead <= 0) {
      throw std::runtime_error("Error receiving data");
    }

    data.resize(bytesRead);

    return data;
  }

  std::expected<Success, SocketError> UDPSocket::sendDecoyConnection(std::string &ip, int port) {
    std::vector<char> data { '0' };

    (*this)[ip, port] << data;

    return Success();
  }

  void UDPSocket::bind(const unsigned int portToBindTo) const {
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

  bool UDPSocket::isIpV6() const {
    return this->_ipv6;
  }

  UDPSocket::~UDPSocket() {
    close(this->_socketId);
  }

  UDPSocket::UdpStream::UdpStream(UDPSocket &udpSocket
                                  , std::string &ip
                                  , int port
                                  , bool broadcast
                                  , size_t readBufferSize) :
      _udpSocket(udpSocket)
      , _sockInfo{}
      , _broadcast(broadcast)
      , _readBufferSize(readBufferSize){

    memset(&(this->_sockInfo), 0, sizeof(sockaddr_in));
    _sockInfo.sin_family = AF_INET;
    _sockInfo.sin_port = htons(static_cast<uint16_t>(port));

    this->_broadcast = broadcast;
    if (!ip.empty()) {
      inet_pton(AF_INET, ip.data(), &(this->_sockInfo.sin_addr));
    } else {
      this->_sockInfo.sin_addr.s_addr = INADDR_ANY;
    }
  }

  UDPSocket::UdpStream &UDPSocket::UdpStream::operator<<(std::vector<char> &data) {
    if (this->_broadcast) {
      this->_udpSocket.setBroadcast(true);
    }

    std::span<char> dataSpan{data};

    this->_udpSocket.sendTo(dataSpan, dataSpan.size(), this->_sockInfo);

    return *this;
  }

  UDPSocket::UdpStream &UDPSocket::UdpStream::operator>>(std::vector<char> &data) {
    size_t bufferSize = data.size();

    if (data.size() < _readBufferSize) {
      data.resize(_readBufferSize);
    }

    std::vector<char> buffer = this->_udpSocket.recvFrom(data.size(), this->_sockInfo);

    if (bufferSize == 0) {
      data = std::move(buffer);
    } else {
      data.resize(buffer.size());
      data.insert(data.end(), buffer.begin(), buffer.end());
    }

    return *this;
  }

  template<typename writeType>
  UDPSocket::UdpStream &UDPSocket::UdpStream::operator<<(const writeType &data) {
    if (this->_broadcast) {
      this->_udpSocket.setBroadcast(true);
    }

    std::ostringstream oss;
    oss << data;

    std::string str = oss.str();
    std::span<char> dataSpan = std::span<char>(str.data(), str.size());

    this->_udpSocket.sendTo(dataSpan, dataSpan.size(), this->_sockInfo);

    return *this;
  }

  template<typename readType>
  UDPSocket::UdpStream &UDPSocket::UdpStream::operator>>(readType &data) {
    if (this->_broadcast) {
      this->_udpSocket.setBroadcast(true);
    }

    std::vector<char> buffer = this->_udpSocket.recvFrom(data.size(), this->_sockInfo);
    std::string str(buffer.begin(), buffer.end());

    std::istringstream iss(str);
    iss >> data;

    return *this;
  }
}