//
// Created by josephvalverde on 12/11/23.
//

#ifndef HTTPSERVER_SSLCONTROLLER_HPP
#define HTTPSERVER_SSLCONTROLLER_HPP

#include <openssl/ssl.h>
#include <memory>
#include <span>
#include <functional>
#include <expected>

#include "../../../Common/Error.hpp"

namespace HarborCLS {
  enum class SSLErrors {
    GENERIC_ERROR,
  };

  using SSLError = Error<SSLErrors>;


  class SSLController {
    std::unique_ptr<SSL_CTX, decltype(&SSL_CTX_free)> SSLContext;
    std::unique_ptr<SSL, std::function<void(SSL * )>> SSLStruct;

    const unsigned int BUFFER_SIZE = 512;

  public:
    /**
     * @brief Creates an SSLController with a server context.
     * @param certFileName The certificate file name.
     * @param keyFileName The key file name.
     */
    SSLController(const std::string &certFileName, const std::string &keyFileName);

    /**
     * @brief Creates an SSLController with a client context.
     */
    SSLController();

    SSLController(SSLController &&) = delete;

    SSLController(const SSLController &) = delete;

    SSLController &operator=(SSLController &&) = delete;

    SSLController &operator=(const SSLController &) = delete;

    /**
     * @brief Initializes a ssl handshake
     * @param socketId id of the socket holding the SSLController
     */
    [[nodiscard]] std::expected<Success, SSLError> SSLConnect(int socketId) const;

    /**
     * @brief Accepts a ssl handshake
     */
    std::expected<Success, SSLError> SSLAccept();

    /**
     * @brief Initializes a ssl connection session
     * @param sslController SSLController that was listening for a connection
     * @param socketFD The socket file descriptor
     */
    std::expected<Success, SSLError> SSLCreate(const SSLController &sslController, int socketFD);

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
    size_t SSLWrite(const std::span<char> &data);

  private:
    void initServerContext(const std::string &certFileName, const std::string &keyFileName);

    void initClientContext();
  };

}

#endif //HTTPSERVER_SSLCONTROLLER_HPP