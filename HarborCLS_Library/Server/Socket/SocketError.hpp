//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_SOCKETERROR_HPP
#define HARBOR_CLS_SOCKETERROR_HPP

#include "../../Common/Error.hpp"

namespace HarborCLS {
  enum class SocketErrors {
    OK = 0,
    GENERIC_ERROR,
    SOCKET_CREATION_ERROR,
    SOCKET_CONNECTION_ERROR,
    SOCKET_BIND_ERROR,
    SOCKET_LISTEN_ERROR,
    SOCKET_ACCEPT_ERROR,
    SOCKET_READ_ERROR,
    SOCKET_WRITE_ERROR,
    SOCKET_TIMEOUT_ERROR,
    SOCKET_FAILED_TO_GET_ADDRESS,
    SOCKET_CONNECTION_REFUSED
  };

  using SocketError = Error<SocketErrors>;
}

#endif //HARBOR_CLS_SOCKETERROR_HPP
