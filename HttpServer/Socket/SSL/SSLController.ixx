//
// Created by josephvalverde on 12/11/23.
//

#include <openssl/ssl.h>
#include <functional>
#include <span>

export module SSLController;

import <memory>;

export class SSLController {
    std::unique_ptr<SSL_CTX, decltype(&SSL_CTX_free)> SSLContext;
    std::unique_ptr<SSL, decltype(&SSL_free)> SSLStruct;

    const unsigned int BUFFER_SIZE = 512;

public:
  SSLController(const std::string& certFileName, const std::string& keyFileName);

  SSLController();

  SSLController(SSLController&&) = delete;
  SSLController(const SSLController&) = delete;
  SSLController& operator=(SSLController&&) = delete;
  SSLController& operator=(const SSLController&) = delete;

  void SSLConnect(int socketId);

  void SSLAccept();

  void SSLCreate(const SSLController* sslController, int socketFD);

  [[nodiscard]] std::pair<std::vector<char>, unsigned int> SSLRead();

  unsigned int SSLWrite(const std::span<char>& data);

private:
    void initServerContext(const std::string& certFileName, const std::string& keyFileName);

    void initClientContext();
};