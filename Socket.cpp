
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
#include <iostream>

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <errno.h>

#include "Socket.hpp"

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
Socket::Socket( char type, bool IPv6) {
  int socketType = 0;
  if (type == 's') {
   socketType = SOCK_STREAM;
  } else {
   socketType = SOCK_DGRAM;
  }
  this->ipv6 = IPv6;
  if (this->ipv6 == false) {
   this->idSocket = socket(AF_INET, socketType, 0);
  } else {
   this->idSocket = socket(AF_INET6, socketType, 0);
  }
   if (this->idSocket < 0) {
      perror( "Socket::Socket" );
      exit(2);
   }

   this->SSLContext = NULL;
   this->SSLStruct = NULL;
}

/**
  * Class destructor
  *
 **/
Socket::~Socket(){
    Close();

   if (this->SSLContext) { 
      SSL_CTX_free( (SSL_CTX *) this->SSLContext );
   }
   if (this->SSLStruct) {
      SSL_free( (SSL *) this->SSLStruct );
   }
}


void Socket::Close(){
    ::close(this->idSocket);
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
   int st = -1;
   if (this->ipv6 == false) {
      struct sockaddr_in  host4;
      struct sockaddr * ha;
      memset( (char *) &host4, 0, sizeof( host4 ) );
      host4.sin_family = AF_INET;
      inet_pton( AF_INET, host, &host4.sin_addr );
      host4.sin_port = htons( port );
      ha = (sockaddr*)&host4;
      st = connect( idSocket, (sockaddr *) ha, sizeof( host4 ) );
   } else {
      struct sockaddr_in6  host6;
      struct sockaddr * ha;
      memset( (char *) &host6, 0, sizeof( host6 ) );
      host6.sin6_family = AF_INET6;
      inet_pton( AF_INET6, host, &host6.sin6_addr );
      host6.sin6_port = htons( port );
      ha = (struct sockaddr *) &host6;
      int len = sizeof( host6 );
      st = connect( idSocket, (sockaddr *) ha, sizeof( host6));

   }

     if ( st == -1 ) {	// check for errors
         perror( "Socket::Connect, address error" );
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
   // st = Connect(host, ::getservbyname(service, "tcp")->s_port);
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
int Socket::Read( void * text, size_t size ) {
   int st = -1;
   st = ::read(this->idSocket, text, size);
   if (st < 0) {
      perror( "Socket::Read" );
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
int Socket::Write( const void *text, size_t size ) {
   int st = -1;
   st = ::write(this->idSocket, text, size);
   if (st < 0) {
      perror( "Socket::Write" );
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
   int st = -1;
   st = ::write(this->idSocket, text, strlen(text));
   if (st < 0) {
      perror( "Socket::Write" );
      exit(2);
   }
   return st;

}

int Socket::sendTo(const void * buffer, size_t bufferSize, void * receptor ) {
   int st = -1;
   // st = send(this->idSocket, buffer, bufferSize, 0);
   //  ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
   //                    const struct sockaddr *dest_addr, socklen_t addrlen);
   st = ::sendto(this->idSocket, buffer, bufferSize, 0,
    (const struct sockaddr *)receptor, sizeof( sockaddr ));
   if (st == -1) {
      perror( "Socket::sendTo" );
      exit(2);
   }
   return st;
}

int Socket::recvFrom(void * buffer, size_t bufferSize, void * receptor) {
   int st = -1;
   // st = recv();
   //  ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
   //                      struct sockaddr *src_addr, socklen_t *addrlen);
   socklen_t addrlen = sizeof(receptor);
   st = ::recvfrom(this->idSocket, buffer, bufferSize, 0,
    (struct sockaddr *)receptor, &addrlen);
   if (st == -1) {
      perror( "Socket::recvFrom" );
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
    int st = -1;

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
   // int bind(int sockfd, const struct sockaddr *addr,
   //              socklen_t addrlen);
   if (this->ipv6 == false) {
      memset( (char *) &host4, 0, sizeof( host4 ) );
      host4.sin_family = AF_INET;
      host4.sin_port = htons( port );
      ha = (sockaddr*)&host4;
      st = ::bind(this->idSocket, (const struct sockaddr *) (sockaddr*)ha, sizeof(host4));
   } else {
      memset( (char *) &host6, 0, sizeof( host6 ) );
      host6.sin6_family = AF_INET6;
      host6.sin6_port = htons( port );
      ha = (sockaddr*)&host6;
      st = ::bind(this->idSocket, (const struct sockaddr *)ha, sizeof(host6));
   }
    if (st == -1) {
      perror( "Socket::Bind" );
      exit(2);
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
Socket * Socket::Accept(){

   return (Socket *) 0;

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
   int st = -1;

   return st;

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


void Socket::InitSSLContext() {
   const SSL_METHOD * method = TLS_client_method();
   SSL_CTX * context = SSL_CTX_new(method);
   if (!context) {
      perror( "Socket::InitSSLContext" );
      exit( 23 );
      Close();
   }
   this->SSLContext = (void *) context;
}

void Socket::InitSSL() {
   this->InitSSLContext();
   SSL * ssl = SSL_new( ((SSL_CTX *) this->SSLContext ) );
   if (!ssl) {
      perror( "Socket::InitSSL" );
      exit( 23 );
      Close();
   }
   this->SSLStruct = (void *) ssl;
}

int Socket::SSLConnect( char * host, int port ) {
   int st = -1;
   this->Connect( host, port );
   SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket );
   st = SSL_connect( (SSL *) this->SSLStruct );
   if ( -1 == st ) {
      perror( "Socket::SSLConnect" );
      exit( 23 );
      Close();
   }
   return st;
}

int Socket::SSLConnect( char * host, char * service){
   int st = -1;
   this->Connect( host, service );
   SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket );
   st = SSL_connect( (SSL *) this->SSLStruct );
   if ( -1 == st ) {
      perror( "Socket::SSLConnect" );
      exit( 23 );
      Close();
   }
   return st;
}

int Socket::SSLRead( void * buffer, int size){
   int st = -1;
   st = SSL_read( (SSL *) this->SSLStruct, buffer, size );
   if ( -1 == st ) {
      perror( "Socket::SSLRead" );
      exit( 23 );
      Close();
   }
   return st;
}

int Socket::SSLWrite( void * buffer, int size){
   int st = -1;
   st = SSL_write( (SSL *) this->SSLStruct, buffer, size );
   if ( -1 == st ) {
      perror( "Socket::SSLWrite" );
      exit( 23 );
      Close();
   }
   return st;
}

