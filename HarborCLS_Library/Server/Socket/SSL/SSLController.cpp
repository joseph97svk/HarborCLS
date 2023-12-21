//
// Created by josephvalverde on 12/11/23.
//

#include <filesystem>
#include "SSLController.hpp"

namespace HarborCLS {

  SSLController::SSLController(const std::string &certFileName, const std::string &keyFileName) :
      SSLContext({nullptr, &SSL_CTX_free}),
      SSLStruct({nullptr, [](SSL *ssl) {
        close(SSL_get_fd(ssl));
        SSL_free(ssl);
      }}) {
    bool missingCertFile = !std::filesystem::exists(certFileName);
    bool missingKeyFile = !std::filesystem::exists(keyFileName);

    if (missingCertFile || missingKeyFile) {
      std::string message;

      message += missingKeyFile ? ("Certificate " + certFileName + " does not exist\n") : "";
      message += missingCertFile ? ("Key " + keyFileName + " does not exist\n") : "";

      throw std::invalid_argument(message);
    }

    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    this->initServerContext(certFileName, keyFileName);
  }

  void SSLController::initServerContext(const std::string &certFileName, const std::string &keyFileName) {
    const SSL_METHOD *method = TLS_server_method();
    if (!method) {
      throw std::runtime_error("SSLController::initServerContext: Failed to create SSL_METHOD");
    }

    this->SSLContext.reset(SSL_CTX_new(method));
    if (!this->SSLContext) {
      throw std::runtime_error("SSLController::initServerContext: Failed to create SSL_CTX");
    }

    if (0 >= SSL_CTX_use_certificate_file(this->SSLContext.get(), certFileName.c_str(), SSL_FILETYPE_PEM)) {
      throw std::runtime_error("SSLController::initServerContext: Failed to set certificate on certificate file name: "
                               + certFileName);
    }

    if (0 >= SSL_CTX_use_PrivateKey_file(this->SSLContext.get(), keyFileName.c_str(), SSL_FILETYPE_PEM)) {
      throw std::runtime_error("SSLController::initServerContext: Failed to set private key");
    }

    if (!SSL_CTX_check_private_key(this->SSLContext.get())) {
      throw std::runtime_error("SSLController::initServerContext: Failed to verify private key");
    }
  }

  SSLController::SSLController() :
      SSLContext({nullptr, &SSL_CTX_free}),
      SSLStruct({nullptr, [](SSL *ssl) {
        close(SSL_get_fd(ssl));
        SSL_free(ssl);
      }}) {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    this->initClientContext();
  }

  void SSLController::initClientContext() {
    const SSL_METHOD *method = TLS_client_method();
    if (!method) {
      throw std::runtime_error("SSLController::SSLController: Failed to create SSL_METHOD");
    }

    this->SSLContext.reset(SSL_CTX_new(method));

    if (!this->SSLContext) {
      throw std::runtime_error("SSLController::SSLController: Failed to create SSL_CTX");
    }

    this->SSLStruct.reset(SSL_new(this->SSLContext.get()));
    if (!this->SSLStruct) {
      throw std::runtime_error("SSLController::SSLController: Failed to create SSL");
    }
  }

  std::expected<Success, SSLError> SSLController::SSLConnect(int socketId) const {
    SSL_set_fd(this->SSLStruct.get(), socketId);

    if (SSL_connect(this->SSLStruct.get()) != 1) {
      return std::unexpected(
          SSLError("SSLController::SSLConnect: Failed to connect SSL"
                   , SSLErrors::GENERIC_ERROR));
    }

    return Success();
  }

  std::expected<Success, SSLError> SSLController::SSLAccept() {
    if (::SSL_accept(this->SSLStruct.get()) != 1) {
      int err = SSL_get_error(this->SSLStruct.get(), err);
      if (!(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)) {
        return std::unexpected(
            SSLError("SSLController::SSLAccept: Failed to accept SSL; handshake failed"
                     , SSLErrors::GENERIC_ERROR));
      }
    }

    return Success();
  }

  std::expected<Success, SSLError> SSLController::SSLCreate(const SSLController &sslController, int socketFD) {
    this->SSLStruct.reset(SSL_new(sslController.SSLContext.get()));

    if (!this->SSLStruct) {
      return std::unexpected(
          SSLError("SSLController::SSLCreate: Failed to create SSL"
                   , SSLErrors::GENERIC_ERROR));
    }

    SSL_set_fd(this->SSLStruct.get(), socketFD);

    return Success();
  }

  std::pair<std::vector<char>, unsigned int> SSLController::SSLRead() const {
    std::vector<char> buffer(this->BUFFER_SIZE);
    int bytesRead = SSL_read(this->SSLStruct.get(), buffer.data(), static_cast<int>(this->BUFFER_SIZE));

    if (bytesRead < 0) {
      int sslError = SSL_get_error(this->SSLStruct.get(), bytesRead);
      if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) {
        return std::make_pair(std::vector<char>(), 0);
      } else {
        throw std::runtime_error("SSLController::SSLRead: Failed to read from SSL");
      }
    }

    return std::make_pair(buffer, bytesRead);
  }

  unsigned int SSLController::SSLWrite(const std::span<char> &data) {
    int bytesWritten = SSL_write(this->SSLStruct.get(), data.data(), static_cast<int>(data.size()));

    if (bytesWritten < 0) {
      int sslError = SSL_get_error(this->SSLStruct.get(), bytesWritten);
      if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) {
        return 0;
      } else {
        throw std::runtime_error("SSLController::SSLWrite: Failed to write to SSL");
      }
    }

    return bytesWritten;
  }
}