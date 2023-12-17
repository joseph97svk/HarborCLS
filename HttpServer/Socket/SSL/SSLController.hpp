//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_SSLCONTROLLER_HPP
#define HTTPSERVER_SSLCONTROLLER_HPP

#include <openssl/ssl.h>
#include "common.hpp"
#include <memory>
#include <span>
#include <functional>

class SSLController {
    std::unique_ptr<SSL_CTX, decltype(&SSL_CTX_free)> SSLContext;
    std::unique_ptr<SSL, std::function<void(SSL*)>> SSLStruct;

    const unsigned int BUFFER_SIZE = 512;

public:
  /**
   * @brief Creates an SSLController with a server context.
   * @param certFileName The certificate file name.
   * @param keyFileName The key file name.
   */
  SSLController(const std::string& certFileName, const std::string& keyFileName);

  /**
   * @brief Creates an SSLController with a client context.
   */
  SSLController();

  SSLController(SSLController&&) = delete;
  SSLController(const SSLController&) = delete;
  SSLController& operator=(SSLController&&) = delete;
  SSLController& operator=(const SSLController&) = delete;

  /**
   * @brief Initializes a ssl handshake
   * @param socketId id of the socket holding the SSLController
   */
  void SSLConnect(int socketId) const;

  /**
   * @brief Accepts a ssl handshake
   */
  void SSLAccept();

  /**
   * @brief Initializes a ssl connection session
   * @param sslController SSLController that was listening for a connection
   * @param socketFD The socket file descriptor
   */
  void SSLCreate(const SSLController& sslController, int socketFD);

  /**
   * @brief Performs a read operation on the socket.
   * @return A pair containing the data read and the number of bytes read.
   */
  [[nodiscard]] std::pair<std::vector<char>, unsigned int> SSLRead() const;

  /**
   * @brief Writes data to the socket.
   * @param data The data to write.
   * @return The number of bytes written.
   */
  unsigned int SSLWrite(const std::span<char>& data);

private:
    void initServerContext(const std::string& certFileName, const std::string& keyFileName);

    void initClientContext();
};

#endif //HTTPSERVER_SSLCONTROLLER_HPP