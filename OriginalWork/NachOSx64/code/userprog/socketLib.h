#include <unistd.h>
#include <cstring>
#include <stdio.h>

#include <arpa/inet.h>	// for inet_pton
#include <sys/types.h>	// for connect 
#include <sys/socket.h>

// for ssl
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <iostream>


int ipv4Connect(const char * host, int port, int idSocket) {
  int st;
  struct sockaddr_in  host4;
  struct sockaddr * ha;

  memset( (char *) &host4, 0, sizeof( host4 ) );
  host4.sin_family = AF_INET;
  st = inet_pton( AF_INET, host, &host4.sin_addr );

  host4.sin_port = htons( port );
  ha = (sockaddr *) &host4;

  st = connect( idSocket, ha, sizeof( host4 ) );

  if ( -1 == st ) {	// check for errors
    perror( "Socket::Connect IpV4" );
  }

  return st;
}

int ipv6Connect(const char * hostip, int port, int idSocket) {
  struct sockaddr_in6  host6;
  struct sockaddr * ha;

  memset( (char *) &host6, 0, sizeof( host6 ) );
  host6.sin6_family = AF_INET6;
  int st = inet_pton( AF_INET6, hostip, &host6.sin6_addr );

  host6.sin6_port = htons( port );
  ha = (struct sockaddr *) &host6;

  st = connect(idSocket, ha, sizeof(host6));

  if ( -1 == st ) {	// 0 means invalid address, -1 means address error
    perror( "Socket::Connect IpV6" );
  }

  return st; 
}

int socketConnect(bool IpV6, const char * ip, int port, int idSocket) {
  int result = 0;
  if (IpV6) {
    result = ipv6Connect(ip, port, idSocket);
  } else {
    result = ipv4Connect(ip, port, idSocket);
  }
  return result;
}

int SSLConnect(bool IpV6, char * ip, int port,
    int idSocket, SSL* SSLStruct) {
  // Establish a non SSL connection first   
  socketConnect(IpV6, ip, port, idSocket);

  int result = SSL_set_fd(SSLStruct, idSocket);

  result = SSL_connect(SSLStruct);

  result = SSL_get_error(SSLStruct, result);

  return result;
}

SSL_CTX * InitSSLContext() {
  // Initialize SSL library
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  // We must create a method to define our context
  const SSL_METHOD * method = TLS_client_method();
  // Check for errors
  return SSL_CTX_new( method );
}

SSL* InitSSL(SSL_CTX*& SSLContext) {
  SSLContext = InitSSLContext();

  return SSL_new((SSL_CTX *) SSLContext);
}

int SSLRead(SSL* SSLStruct, void * message, int messageSize) {
  return SSL_read(SSLStruct, message, messageSize);
}

int SSLWrite(SSL* SSLStruct, const void * message, int messageSize) {
  return SSL_write(SSLStruct, message, messageSize);
}