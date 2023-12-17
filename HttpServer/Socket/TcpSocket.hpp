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

  bool ipv6;
  bool ssl;

  std::optional<SSLController> sslController;

  const unsigned int BUFFER_SIZE = 512;
public:
  /**
   * @brief Creates a TcpSocket object with the given parameters.
   * @param ipv6 Whether or not to use ipv6.
   */
  explicit TcpSocket(bool ipv6);

  /**
   * @brief Creates a TcpSocket object with the given parameters.
   * @param certFileName The name of the certificate file.
   * @param keyFileName The name of the key file.
   * @param ipv6 Whether or not to use ipv6.
   */
  TcpSocket(const std::string& certFileName, const std::string& keyFileName, bool ipv6);

  /**
   * @brief Creates a TcpSocket object with the given parameters.
   * @param socketFd The socket file descriptor.
   */
  explicit TcpSocket(int socketFd);

  //NO_COPY(TcpSocket)

  /**
   * @brief Destroys the TcpSocket object.
   */
  ~TcpSocket() = default;

  /**
   * @brief Connects to the given host and port.
   * @param host The host to connect to.
   * @param targetPort The port to connect to.
   */
  void connect(const std::string& host, int targetPort);

  /**
   * @brief Connects to the given host and service.
   *
   * @param host The host to connect to.
   * @param service The service to connect to.
   */
  void connect(const std::string& host, const std::string& service) const;

  /**
   * @brief Closes the socket.
   */
  void close() const;

  /**
   * @brief Sends the given message.
   * @tparam readDatatype type of the data to be read.
   * @param data where the data will be stored.
   * @return
   */
  template<typename readDatatype>
  TcpSocket& operator>>(readDatatype& data);

  /**
   * @brief Receives a message.
   * @tparam writeDatatype type of the data to be written.
   * @param data data to be sent.
   * @return
   */
  template<typename writeDatatype>
  TcpSocket& operator<<(const writeDatatype& data);

  /**
   * @brief Sends the given message.
   * @param data vector of data to be sent.
   * @return
   */
  TcpSocket& operator<<(std::vector<char>& data);

  /**
   * @brief Receives a message.
   * @param data where the data will be stored.
   * @return
   */
  TcpSocket& operator>>(std::vector<char>& data);

  /**
   * @brief Binds the socket to the given port.
   * @param portToBindTo The port to bind to.
   */
  void bind(unsigned int portToBindTo) const;

  /**
   * @brief Sets the socket to listen for connections
   * @param queueSize Amount of connections to be held in _queue.
   */
  void listen(unsigned int queueSize) const;

  /**
   * @brief Accepts a connection.
   * @return a shared pointer to the socket of the accepted connection.
   */
  [[nodiscard]] std::shared_ptr<TcpSocket> accept() const;

  /**
   * Returns whether or not the socket is SSL.
   * @return bool of whether or not the socket is SSL.
   */
  [[nodiscard]] bool isSSL() const;

  /**
   * @brief Sets the timeout for the socket.
   * @param seconds to wait for timeout.
   * @param microseconds to wait for timeout.
   */
  void setTimeout(size_t seconds, size_t microseconds) const;

  static char getType() {
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