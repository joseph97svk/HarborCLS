//
// Created by josephvalverde on 12/12/23.
//

#include <stdexcept>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>
#include "UdpSocket.hpp"

namespace HarborCLS {

  UDPSocket::UDPSocket(bool ipv6)
    : _ipv6(ipv6) {

    if (_ipv6) {
      this->socketId = ::socket(AF_INET6, SOCK_DGRAM, 0);
    } else {
      this->socketId = ::socket(AF_INET, SOCK_DGRAM, 0);
    }

    if (socketId < 0) {
      throw std::runtime_error("Error creating socket");
    }
  }

  UDPSocket::UdpStream UDPSocket::operator[](std::string &ip, int port) {
    bool broadcast = false;
    if (!ip.empty()) {
      broadcast = ip[0] == '-';
    }
    if (broadcast) {
      ip = ip.substr(1, ip.size());
    }

    return {*this, ip, port, broadcast};
  }

  void UDPSocket::setBroadcast(const bool broadcast) const {
    int broadcastEnable = static_cast<int>(broadcast);

    if (setsockopt(
        this->socketId,
        SOL_SOCKET,
        SO_BROADCAST,
        &broadcastEnable,
        sizeof(broadcastEnable)) < 0) {
      throw std::runtime_error("Error setting broadcast");
    }
  }

  inline void UDPSocket::sendTo(const std::span<char> &data, size_t size, const sockaddr_in &sockInfo) const {
    if (sendto(
        this->socketId,
        data.data(),
        size,
        0,
        reinterpret_cast<const sockaddr *>(&sockInfo),
        sizeof(sockInfo)) < 0) {
      throw std::runtime_error("Error sending data");
    }
  }

  inline std::vector<char> UDPSocket::recvFrom(size_t size, sockaddr_in &sockInfo) const {
    std::vector<char> data(size);

    if (recvfrom(
        this->socketId,
        data.data(),
        size,
        0,
        reinterpret_cast<sockaddr *>(&sockInfo),
        reinterpret_cast<socklen_t *>(sizeof(sockaddr))) < 0) {
      throw std::runtime_error("Error receiving data");
    }

    return data;
  }

  std::expected<Success, SocketError> UDPSocket::sendDecoyConnection(std::string &ip, int port) {
    std::vector<char> data;
    data.push_back('0');

    (*this)[ip, port] << data;

    return Success();
  }

  bool UDPSocket::isIpV6() const {
    return this->_ipv6;
  }

  UDPSocket::UdpStream::UdpStream(UDPSocket &udpSocket, std::string &ip, int port, bool broadcast) :
      udpSocket(udpSocket),
      broadcast(broadcast),
      sockInfo{} {

    memset(&(this->sockInfo), 0, sizeof(sockaddr_in));
    sockInfo.sin_family = AF_INET;
    sockInfo.sin_port = htons(port);

    this->broadcast = broadcast;
    if (!ip.empty()) {
      inet_pton(AF_INET, ip.data(), &(this->sockInfo.sin_addr));
    } else {
      this->sockInfo.sin_addr.s_addr = INADDR_ANY;
    }
  }

  UDPSocket::UdpStream &UDPSocket::UdpStream::operator<<(std::vector<char> &data) {
    if (this->broadcast) {
      this->udpSocket.setBroadcast(true);
    }

    std::span<char> dataSpan{data};

    this->udpSocket.sendTo(dataSpan, dataSpan.size(), this->sockInfo);

    return *this;
  }

  UDPSocket::UdpStream &UDPSocket::UdpStream::operator>>(std::vector<char> &data) {
    if (this->broadcast) {
      this->udpSocket.setBroadcast(true);
    }

    std::vector<char> buffer = this->udpSocket.recvFrom(data.size(), this->sockInfo);

    if (data.empty()) {
      data = std::move(buffer);
    } else {
      data.insert(data.end(), buffer.begin(), buffer.end());
    }

    return *this;
  }

  template<typename writeType>
  UDPSocket::UdpStream &UDPSocket::UdpStream::operator<<(const writeType &data) {
    if (this->broadcast) {
      this->udpSocket.setBroadcast(true);
    }

    std::ostringstream oss;
    oss << data;

    std::string str = oss.str();
    std::span<char> dataSpan = std::span<char>(str.data(), str.size());

    this->udpSocket.sendTo(dataSpan, dataSpan.size(), this->sockInfo);

    return *this;
  }

  template<typename readType>
  UDPSocket::UdpStream &UDPSocket::UdpStream::operator>>(readType &data) {
    if (this->broadcast) {
      this->udpSocket.setBroadcast(true);
    }

    std::vector<char> buffer = this->udpSocket.recvFrom(data.size(), this->sockInfo);
    std::string str(buffer.begin(), buffer.end());

    std::istringstream iss(str);
    iss >> data;

    return *this;
  }
}