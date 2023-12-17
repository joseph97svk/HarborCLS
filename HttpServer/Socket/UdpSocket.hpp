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
  /**
   * @brief Creates a UDPSocket object with the given parameters.
   * @param ipv6 whether or not to use ipv6. If false, ipv4 is used.
   */
  explicit UDPSocket(bool ipv6 = false);

  /**
   * @brief temporary object to ease comunications
   */
  class UdpStream {
    UDPSocket& udpSocket;
    sockaddr_in sockInfo;
    bool broadcast;

  public:
    /**
     * @brief Creates a UdpStream object with the given parameters.
     * @param udpSocket
     * @param ip
     * @param port
     * @param broadcast
     */
    UdpStream(UDPSocket& udpSocket, std::string& ip, int port, bool broadcast);

    /**
     * @brief Sends the given data to the ip and port specified in the constructor.
     * @param data The data to send.
     */
    template <typename readType>
    UdpStream& operator>>(readType& data);

    /**
     * @brief Receives data from the ip and port specified in the constructor.
     * @param data Where to store the received data.
     */
    template <typename writeType>
    UdpStream& operator<<(const writeType& data);

    /**
     * @brief Sends the given data to the ip and port specified in the constructor.
     * @param data The data to send.
     */
    UdpStream& operator<<(std::vector<char>& data);

    /**
     * @brief Receives data from the ip and port specified in the constructor.
     * @param data Where to store the received data.
     */
    UdpStream& operator>>(std::vector<char>& data);
  };

  /**
   * @brief Creates a UdpStream
   * @param ip to send data to or receive data from
   * @param port to send data to or receive data from
   * @return UdpStream object
   */
  UdpStream operator[](std::string& ip, int port);

private:
  void setBroadcast(bool broadcast) const;

  void sendTo(const std::span<char>& data, size_t size, const sockaddr_in& sockInfo) const;

  [[nodiscard]] std::vector<char> recvFrom(size_t size, sockaddr_in& sockInfo) const;
};

#endif //HTTPSERVER_UDPSOCKET_HPP