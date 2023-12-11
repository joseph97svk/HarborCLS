/**
 *   CI0123 PIRO
 *   Clase para utilizar los "sockets" en Linux
 *
 **/

#include <stdio.h>	// for perror
#include <stdlib.h>	// for exit
#include <string.h>	// for memset
#include <arpa/inet.h>	// for inet_pton
#include <sys/types.h>	// for connect 
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <memory>
#include <iostream>

#include "Socket.hpp"

int ipv4Connect(const char * host, int port, int idSocket);

int ipv6Connect(const char * hostip, int port, int idSocket);

/**
  *  Class constructor
  *     use Unix socket system call
  *
  *  @param	char type: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param	bool ipv6: if we need a IPv6 socket
  *
 **/
Socket::Socket( char type, bool IPv6 ){
  int sockType = 0;

  this->type = type;

  switch(type) {
    case 's':
      sockType = SOCK_STREAM;
      break;
    case 'd':
      sockType = SOCK_DGRAM;
      break;
  }

  this->ipv6 = IPv6;

  if (IPv6) {
    this->idSocket = socket(AF_INET6, sockType, 0);
  } else {
    this->idSocket = socket(AF_INET, sockType, 0);
  }

  if (this->idSocket < 0) {
    // Error handling
    perror("Socket::Socket");
    exit(1);
  }

  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
}

Socket::Socket(int socketFD) {
  this->idSocket = socketFD;

  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
}


/**
  * Class destructor
  *
 **/
Socket::~Socket(){
  Close();
}


/**
  * Close method
  *    use Unix close system call (once opened a socket is managed like a file in Unix)
  *
 **/
void Socket::Close(){
  if (this->SSLStruct != nullptr) {

    int fileDescriptor = SSL_get_fd((SSL*) this->SSLStruct);

    SSL_free((SSL*) this->SSLStruct);

    bool same = this->idSocket == fileDescriptor;

    this->SSLStruct = nullptr;
    if (fileDescriptor >= 0) {
      close(fileDescriptor);

      fileDescriptor = -1;
    }
    if (same) {
      return;
    }
  }

  close(this->idSocket);
}

/**
  * Connect method
  *   use "connect" Unix system call
  *
  * @param	char * host: host address in dot notation, example "10.1.104.187"
  * @param	int port: process address, example 80
  *
 **/
int Socket::Connect( const char * host, int port ) {
  // for ipv6 case
  if (this->ipv6) {
    return ipv6Connect(host, port, this->idSocket);
  }

  // for ipv4 case
  return ipv4Connect(host, port, this->idSocket);
}

int ipv4Connect(const char * host, int port, int idSocket) {
  int st;
  struct sockaddr_in  host4;
  struct sockaddr * ha;

  memset( (char *) &host4, 0, sizeof( host4 ) );
  host4.sin_family = AF_INET;
  st = inet_pton( AF_INET, host, &host4.sin_addr );
  /*
  if ( 1 != st ) {	// 0 means invalid address, -1 means address error
    perror( "Socket::Connect, inet_pton IpV4" );
    std::cout << st << std::endl;
    exit( 2 );
  } */
  
  host4.sin_port = htons( port );
  ha = (sockaddr *) &host4;
  st = connect( idSocket, ha, sizeof( host4 ) );

  if ( -1 == st ) {	// check for errors
    perror( "Socket::Connect IpV4" );
    exit( 2 );
  }

  return st;
}

int ipv6Connect(const char * hostip, int port, int idSocket) {
  struct sockaddr_in6  host6;
  struct sockaddr * ha;

  memset( (char *) &host6, 0, sizeof( host6 ) );
  host6.sin6_family = AF_INET6;
  int st = inet_pton( AF_INET6, hostip, &host6.sin6_addr );
  /*
  if ( 1 != st ) {	// 0 means invalid address, -1 means address error
    perror( "Socket::Connect, inet_pton IpV6" );
    std::cout << st << std::endl;
    exit( 2 );
  } */

  host6.sin6_port = htons( port );
  ha = (struct sockaddr *) &host6;

  st = connect(idSocket, ha, sizeof(host6));

  if ( -1 == st ) {	// 0 means invalid address, -1 means address error
    perror( "Socket::Connect IpV6" );
    std::cout << st << std::endl;
    exit( 2 );
  }

  return st; 
}
 

/**
  * Connect method
  *
  * @param	char * host: host address in dot notation, example "163.178.104.187"
  * @param	char * service: service name, example "http"
  *
 **/
int Socket::Connect( const char *host, const char *service ) {
  int st = -1;

  struct sockaddr_in6  host6;

  memset( (char *) &host6, 0, sizeof( host6 ) );
  host6.sin6_family = AF_INET6;
  st = inet_pton( AF_INET6, host, &host6.sin6_addr );

  if ( 1 == st ) {	// 0 means invalid address, -1 means address error
    perror( "Socket::Connect, inet_pton" );
    exit( 2 );
  }

  host6.sin6_port = htons( port );
  struct addrinfo hints, *result, *rp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Stream socket */
  hints.ai_flags = 0;
  hints.ai_protocol = 0;          /* Any protocol */

  st = getaddrinfo( host, service, &hints, &result );

  for ( rp = result; rp; rp = rp->ai_next ) {
    st = connect( idSocket, rp->ai_addr, rp->ai_addrlen );
    if ( 0 == st )
        break;
  }

  freeaddrinfo( result );

  return st;
}


/**
  * Read method
  *   use "read" Unix system call (man 3 read)
  *
  * @param	void * text: buffer to store data read from socket
  * @param	int size: buffer capacity, read will stop if buffer is full
  *
 **/
int Socket::Read( void * buffer, int bufferSize ) {
  int st = read(this->idSocket, buffer, bufferSize);

  // checkear status
  if (-1 == st) {
    perror("Socket::Read");
    exit(2);
  }

  return st;
}


/**
  * Write method
  *   use "write" Unix system call (man 3 write)
  *
  * @param	void * buffer: buffer to store data write to socket
  * @param	size_t size: buffer capacity, number of bytes to write
  *
 **/
int Socket::Write( const void * buffer, int bufferSize ) {
  int st = write(this->idSocket, buffer, bufferSize);

  // checkear status
  if (-1 == st) {
    perror("Socket::Write");
    exit(2);
  }

  return st;
}


/**
  * Write method
  *
  * @param	char * text: string to store data write to socket
  *
  *  This method write a string to socket, use strlen to determine how many bytes
  *
 **/
int Socket::Write( const char *text ) {
  int st = write(this->idSocket, text, strlen(text));

  // checkear status
  if (-1 == st) {
    perror("Socket::Write");
    exit(2);
  }

  return st;
}


/**
  * Listen method
  *   use "listen" Unix system call (server mode)
  *
  * @param	int queue: max pending connections to enqueue 
  *
  *  This method define how many elements can wait in queue
  *
 **/
int Socket::Listen( int queue ) {
  int st = listen(this->idSocket, queue);

  if (st < 0) {
    perror( "Socket::Bind" );
    exit( 4 );
  }

  return st;
}


/**
  * Bind method
  *    use "bind" Unix system call (man 3 bind) (server mode)
  *
  * @param	int port: bind a socket to a port defined in sockaddr structure
  *
  *  Links the calling process to a service at port
  *
 **/
int Socket::Bind( int port ) {
  int st = -1;
  struct sockaddr * ha;
  struct sockaddr_in host4;
  struct sockaddr_in6 host6;

  socklen_t size = 0;

  if (this->ipv6) {
    memset(&host6, 0, sizeof(host6));
    host6.sin6_family = AF_INET6;
    host6.sin6_port = htons(port);
    host6.sin6_addr = in6addr_any;
    ha = (sockaddr*) &host6;
    size = sizeof(host6);
  } else {
    memset(&host4, 0, sizeof(host4));
    host4.sin_family = AF_INET;
    host4.sin_port = htons(port);
    host4.sin_addr.s_addr = INADDR_ANY;
    ha = (sockaddr*) &host4;
    size = sizeof(host4);
  }

  st = bind(this->idSocket, (sockaddr*) ha,
      size);

  if ( -1 == st ) {	// check for errors
    perror( "Socket::Bind" );
    exit( 4 );
  }

  return st;
}


/**
  * Accept method
  *    use "accept" Unix system call (man 3 accept) (server mode)
  *
  *  @returns	a new class instance
  *
  *  Waits for a new connection to service (TCP  mode: stream)
  *
 **/
// Socket * Socket::Accept(){
//   struct sockaddr_in server;
//   socklen_t addr_len = sizeof(server);

//   // accept system call
//   int socketFD =
//       accept(this->idSocket, // file descriptor of this existing socket
//       (sockaddr*) &server, 
//       &addr_len 
//       );

//   if (-1 == socketFD) {  // check for errors
//     // in case the socket is closed abruptly, no need to mention error
//     if (errno != 9) {
//       perror("Socket::Accept");
//     }
//   }
   
//   return new Socket(socketFD);
// }

std::shared_ptr<Socket> Socket::Accept() {
  struct sockaddr_in server;
  socklen_t addr_len = sizeof(server);

  // accept system call
  int socketFD =
      accept(this->idSocket, // file descriptor of this existing socket
      (sockaddr*) &server, 
      &addr_len 
      );

  if (-1 == socketFD) {  // check for errors
    // in case the socket is closed abruptly, no need to mention error
    if (errno != 9) {
      perror("Socket::Accept");
    }
  }
   
  return std::make_shared<Socket>(socketFD);
}


/**
  * Shutdown method
  *    use "shutdown" Unix system call (man 3 shutdown)
  *
  *  @param	int mode define how to cease socket operation
  *
  *  Partial close the connection (TCP mode)
  *
 **/
int Socket::Shutdown( int mode ) {
  return shutdown(this->idSocket, mode);
}


/**
  *  SetIDSocket
  *
  *  @param	int id assigns a new value to field
  *  
 **/
void Socket::SetIDSocket(int id){
  this->idSocket = id;
}

int Socket::sendTo(void* message, int messageSize, void* socketAddress) {
  if (this->ipv6) {
    return sendto(this->idSocket, message, messageSize, 0,
        (struct sockaddr*) socketAddress, sizeof(sockaddr_in6));
  }

  return sendto(this->idSocket, message, messageSize, 0,
      (struct sockaddr*) socketAddress, sizeof(sockaddr));
}

int Socket::recvFrom(void* message, int messageSize, void* socketAddress) {
  socklen_t socketLength = sizeof(sockaddr_in6);
  if (this->ipv6) {
    socketLength = sizeof(sockaddr);
  }
  int bytes = recvfrom(this->idSocket, message, messageSize, 0,
      (struct sockaddr*) socketAddress, &socketLength);
  return bytes;
}

void Socket::InitSSL() {
  this->InitSSLContext();

  SSL * ssl = SSL_new((SSL_CTX *) this->SSLContext);

  // Check for errors
  this->SSLStruct = (void *) ssl;
}

void Socket::InitSSLContext() {
  // Initialize SSL library
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  // We must create a method to define our context
  const SSL_METHOD * method = TLS_client_method();
  // Check for errors
  SSL_CTX * context = SSL_CTX_new( method );
  // Check for errors
  this->SSLContext = (void *) context;
}

void Socket::SSLInitServerContext() {
  // Initialize SSL library
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  // Create SSL/TLS method for server
  const SSL_METHOD* method = TLS_server_method();
  if (method == nullptr) {
    ERR_print_errors_fp(stderr);
    abort();
  }

  // Create SSL context
  SSL_CTX * context = SSL_CTX_new(method);
  if (context == nullptr) {
    ERR_print_errors_fp(stderr);
    abort();
  }

  this->SSLContext = (void*) context;
}

void Socket::SSLInitServer(const char* certificateFileName,
      const char* keyFileName) {

  this->SSLInitServerContext();
  /*
  SSL* ssl = SSL_new(((SSL_CTX*) this->SSLContext));
  this->SSLStruct = ssl;
  SSL_set_fd((SSL*) this->SSLStruct, this->idSocket);
  */
  this->SSLLoadCertificates(certificateFileName, keyFileName);
}

void Socket::SSLLoadCertificates(const char * certFileName,
    const char * keyFileName) {
  int st = 0;

  SSL_CTX* context = (SSL_CTX*) this->SSLContext;

  if (SSL_CTX_use_certificate_file(context,
      certFileName,
      SSL_FILETYPE_PEM)
      <= 0 ) {	 // set the local certificate from CertFile

    st = SSL_get_error((SSL*) this->SSLStruct, st);

    ERR_print_errors_fp(stderr);
    abort();
  }

  if (SSL_CTX_use_PrivateKey_file(context,
      keyFileName,
      SSL_FILETYPE_PEM)
      <= 0 ) {	// set the private key from KeyFile (may be the same as CertFile)
    st = SSL_get_error((SSL*) this->SSLStruct, st);

    ERR_print_errors_fp(stderr);
    abort();
  }

  if (!SSL_CTX_check_private_key(context) ) {	// verify private key
    st = SSL_get_error((SSL*) this->SSLStruct, st);

    ERR_print_errors_fp(stderr);
    abort();
  }
}

void Socket::SSLCreate(Socket* parent) {
  SSL* ssl = SSL_new((SSL_CTX*) parent->SSLContext);

  this->SSLStruct = (void*) ssl;

  SSL_set_fd(ssl, this->idSocket);
}

void Socket::SSLShowCerts() {
  X509 *cert;
  char *line;

  cert = SSL_get_peer_certificate((SSL *) this->SSLStruct);		 // Get certificates (if available)

  if (nullptr != cert) {
    printf("Server certificates:\n");
    line = X509_NAME_oneline(X509_get_subject_name( cert ), 0, 0);
    printf("Subject: %s\n", line);
    free(line);
    line = X509_NAME_oneline(X509_get_issuer_name( cert ), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);
    X509_free(cert);
  } else {
    printf("No certificates.\n");
  }
}

const char* Socket::SSLGetCipher() {
  SSL* ssl = (SSL*) this->SSLStruct;

  // Call SSL_get_cipher() and return the name
  return SSL_get_cipher(ssl);
}

const char* Socket::SSLAccept() {
  SSL* ssl = (SSL*) this->SSLStruct;

  int ret = SSL_accept(ssl); // Perform the SSL/TLS handshake
  if (ret != 1) {
    // Handle error during handshake
    int err = SSL_get_error(ssl, ret);
    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
      // The SSL_accept() call needs to be retried
      return "SSL handshake in progress";
    } else {
      // An error occurred during the handshake
      return "SSL handshake failed";
    }
  }

  // SSL handshake was successful
  return "SSL handshake succeeded";
}

int Socket::SSLConnect(char * host, int port) {
  int st = 0;

  Connect( host, port );	// Establish a non SSL connection first

  SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket );

  st = SSL_connect( (SSL *) this->SSLStruct );
  // check for errors

  return st;
}

int Socket::SSLConnect(char * host, char * service) {
  int st = 0;

  st = this->Connect(host, service);

  SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket );
  
  st = SSL_connect( (SSL *) this->SSLStruct );
  // check for errors

  return st;
}

int Socket::SSLRead(void * message, int messageSize) {
  return SSL_read((SSL*) this->SSLStruct, message, messageSize);
}

int Socket::SSLWrite(const void * message, int messageSize) {
  return SSL_write((SSL*) this->SSLStruct, message, messageSize);
}

int Socket::SSLWrite(const void * message) {
  return SSL_write((SSL*) this->SSLStruct, message, strlen((char*) message));
}


bool Socket::isSSL() {
  return this->SSLStruct != nullptr || this->SSLContext != nullptr;
}


void Socket::setBufferDefault(int size) {
  this->bufferDefaultSize = size;
}