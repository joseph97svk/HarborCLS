//
// Created by josephvalverde on 12/12/23.
//

#ifndef HTTPSERVER_UDPSOCKET_HPP
#define HTTPSERVER_UDPSOCKET_HPP

#include <string>
#include <vector>
#include <span>

#include <netinet/in.h>

class UDPSocket {
  int socketId;

public:
  explicit UDPSocket(bool ipv6 = false);

  class UdpStream {
    UDPSocket& udpSocket;
    sockaddr_in sockInfo;
    bool broadcast;

  public:
    UdpStream(UDPSocket& udpSocket, std::string& ip, int port, bool broadcast);

    template <typename readType>
    UdpStream& operator>>(readType& data);

    template <typename writeType>
    UdpStream& operator<<(const writeType& data);

    UdpStream& operator<<(std::vector<char>& data);

    UdpStream& operator>>(std::vector<char>& data);
  };

  UdpStream operator[](std::string& ip, int port);

private:
    void setBroadcast(bool broadcast) const;

    void sendTo(const std::span<char>& data, size_t size, const sockaddr_in& sockInfo) const;
    [[nodiscard]] std::vector<char> recvFrom(size_t size, sockaddr_in& sockInfo) const;
};

#endif //HTTPSERVER_UDPSOCKET_HPP