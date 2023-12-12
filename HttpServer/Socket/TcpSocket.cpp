//
// Created by josephvalverde on 12/11/23.
//

#include <bits/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "TcpSocket.ixx"

TcpSocket::TcpSocket(const int port, const bool ipv6) :
        socketId(0),
        port(port),
        ipv6(ipv6),
        ssl(false) {
  this->setSocket();
}

TcpSocket::TcpSocket(const int port,
                     const std::string& certFileName,
                     const std::string& keyFileName,
                     const bool ipv6 = false) :
                     socketId(0),
                     port(port),
                     ipv6(ipv6),
                     ssl(true),
                     sslController(std::make_unique<SSLController>(certFileName, keyFileName)){
  this->setSocket();
}

void TcpSocket::setSocket() {
  if (this->ipv6) {
    this->socketId = ::socket(AF_INET6, SOCK_STREAM, 0);
  } else {
    this->socketId = ::socket(AF_INET, SOCK_STREAM, 0);
  }

  if (this->socketId == -1) {
    throw std::runtime_error("TcpSocket::TcpSocket: Failed to create socket");
  }
}

TcpSocket::TcpSocket(const int socketFd) :
        socketId(socketFd),
        port(0),
        ipv6(false),
        ssl(false){
}

void TcpSocket::connect(const std::string& host, const int targetPort) {
  if (this->ssl) {
    this->sslController->SSLConnect(this->socketId);
  }

  if (this->ipv6) {
    this->ipv6Connect(host, targetPort);
  } else {
    this->ipv4Connect(host, targetPort);
  }
}

void TcpSocket::ipv4Connect(const std::string &host, int port) const {
  struct sockaddr * ha;

  struct sockaddr_in host4 {};
  memset( (char *) &host4, 0, sizeof( host4 ) );
  host4.sin_family = AF_INET;

  if( inet_pton( AF_INET, host.c_str(), &host4.sin_addr ) == -1 ) {
    throw std::runtime_error("TcpSocket::connect: Failed to convert host to IPv4 address");
  }

  host4.sin_port = htons( port );
  ha = (struct sockaddr *) &host4;

  if ( ::connect(this->socketId, ha, sizeof( host4 )) == -1 ) {
    throw std::runtime_error("TcpSocket::connect: Failed to connect to host");
  }
}

void TcpSocket::ipv6Connect(const std::string &host, int port) const {
  struct sockaddr * ha;
  struct sockaddr_in6 host6 {};
  memset( (char *) &host6, 0, sizeof( host6 ) );
  host6.sin6_family = AF_INET6;

  if ( inet_pton( AF_INET6, host.c_str(), &host6.sin6_addr ) == -1 ) {
    throw std::runtime_error("TcpSocket::connect: Failed to convert host to IPv6 address");
  }

  host6.sin6_port = htons( port );
  ha = (struct sockaddr *) &host6;

  if ( ::connect(this->socketId, ha, sizeof (host6)) == -1 ) {
    throw std::runtime_error("TcpSocket::connect: Failed to connect to host");
  }
}

void TcpSocket::connect(const std::string& host, const std::string& service) const {
  if (!this->ipv6) {
    throw std::runtime_error("TcpSocket::connect: IPv4 not supported for connect by service");
  }

  if (this->ssl) {
    this->sslController->SSLConnect(this->socketId);
  }

  struct sockaddr_in6  host6 {};

  memset( (char *) &host6, 0, sizeof( host6 ) );
  host6.sin6_family = AF_INET6;
  if (inet_pton( AF_INET6, host.c_str(), &host6.sin6_addr ) == 1) {
    throw std::runtime_error("TcpSocket::connect: Failed to convert host to IPv6 address");
  }

  host6.sin6_port = htons( port );
  struct addrinfo hints{}, *result, *rp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Stream socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */

  if(getaddrinfo( host.c_str(), service.c_str(), &hints, &result ) != 0) {
    throw std::runtime_error("TcpSocket::connect: Failed to get address info");
  }

  for ( rp = result; rp; rp = rp->ai_next ) {
    if(::connect( this->socketId, rp->ai_addr, rp->ai_addrlen ) == 0)
      break;
  }

  freeaddrinfo( result );
}

void TcpSocket::close() const {
  ::close(this->socketId);
}

template<typename readDatatype>
TcpSocket& TcpSocket::operator>>(readDatatype& data) {
  size_t bytesRead = 0;

  std::vector<char> buffer;
  std::pair<std::vector<char>, unsigned int> readData;

  do {
    readData = this->ssl ?
               this->sslController->SSLRead() :
               this->Read();

    bytesRead = readData.second;
    buffer.insert(buffer.end(), readData.first.begin(), readData.first.end());
  } while (bytesRead != data.size());

  data = readDatatype(buffer.begin(), buffer.end());

  return *this;
}

template<typename writeDatatype>
TcpSocket& TcpSocket::operator<<(const writeDatatype& data) {
  std::span<char> dataSpan = std::span<char>(data.data(), data.size());

  if (this->ssl) {
    this->sslController->SSLWrite(dataSpan);
  } else {
    this->Write(dataSpan);
  }

  return *this;
}

void TcpSocket::bind(const int portToBindTo) const {
  struct sockaddr * ha;
  struct sockaddr_in host4{};
  struct sockaddr_in6 host6{};

  socklen_t size = 0;

  if (this->ipv6) {
    memset(&host6, 0, sizeof(host6));
    host6.sin6_family = AF_INET6;
    host6.sin6_port = htons(portToBindTo);
    host6.sin6_addr = in6addr_any;
    ha = (sockaddr*) &host6;
    size = sizeof(host6);
  } else {
    memset(&host4, 0, sizeof(host4));
    host4.sin_family = AF_INET;
    host4.sin_port = htons(portToBindTo);
    host4.sin_addr.s_addr = INADDR_ANY;
    ha = (sockaddr*) &host4;
    size = sizeof(host4);
  }

  if (::bind(this->socketId, (sockaddr*) ha, size) == -1) {
    throw std::runtime_error("TcpSocket::bind: Failed to bind to port");
  }
}

void TcpSocket::listen(const int queueSize) const {
  if (::listen(this->socketId, queueSize) == -1) {
    throw std::runtime_error("TcpSocket::listen: Failed to listen on socket");
  }
}

std::shared_ptr<TcpSocket> TcpSocket::accept() const {
  struct sockaddr_in server{};
  socklen_t addr_len = sizeof(server);

  int socketFD =
          ::accept(this->socketId,
                 (sockaddr*) &server,
                 &addr_len
          );

  if (socketFD == -1) {
    throw std::runtime_error("TcpSocket::accept: Failed to accept connection");
  }

  std::shared_ptr<TcpSocket> acceptedConnection = std::make_shared<TcpSocket>(socketFD);

  if (this->ssl) {
    acceptedConnection->sslController->SSLCreate(this->sslController.get(), socketFD);
    acceptedConnection->sslController->SSLAccept();
  }

  return acceptedConnection;
}

bool TcpSocket::isSSL() {
  return this->ssl;
}

void TcpSocket::Write(const std::span<char>& message) const {
  size_t bytesWritten = ::write(this->socketId, message.data(), message.size());

  if (bytesWritten != message.size()) {
    throw std::runtime_error("TcpSocket::Write: Failed to write all bytes to socket");
  }
}

std::pair<std::vector<char>, unsigned int> TcpSocket::Read() const {
  std::vector<char> buffer(BUFFER_SIZE);
  size_t bytesRead = ::read(this->socketId, buffer.data(), buffer.size());

  return std::make_pair(buffer, bytesRead);
}
