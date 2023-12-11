/**
 *  Establece la definición de la clase Socket para efectuar la comunicación
 *  de procesos que no comparten memoria, utilizando un esquema de memoria
 *  distribuida.  El desarrollo de esta clase se hará en varias etapas, primero
 *  los métodos necesarios para los clientes, en la otras etapas los métodos para el servidor,
 *  manejo de IPv6, conexiones seguras y otros
 *
 *  Universidad de Costa Rica
 *  ECCI
 *  CI0123 Proyecto integrador de redes y sistemas operativos
 *  2023-i
 *  Grupos: 2 y 3
 *
 * (versión Ubuntu)
 *
 **/

#ifndef Socket_h
#define Socket_h

class Socket {

 public:
  Socket( char type, bool IPv6 = false );	// char == 's' => stream, char == 'd' => datagram
  Socket( int socketFD);				// int = socket descriptor
  ~Socket();
  int Connect( const char * host, int port );
  int Connect( const char * host, const char * service );
  void Close();
  int Read( void * buffer, int bufferSize );
  int Write( const void * buffer, int bufferSize );
  int Write( const char * buffer );	// size is calculated with strlen
  int Listen( int backlog );
  int Bind( int port );
  Socket * Accept();
  int Shutdown( int mode );		// mode = { SHUT_RD, SHUT_WR, SHUT_RDWR }
  void SetIDSocket( int newId );
  int sendTo(void* message, int messageSize, void* socketAddress);
  int recvFrom(void* message, int messageSize, void* socketAddress);

  void InitSSL();
  void SSLInit() {
    InitSSL();
  }
  void InitSSLContext();
  void SSLInitServerContext();
  void SSLInitServer(
      const char* certificateFileName,
      const char* keyFileName);
  void SSLLoadCertificates(const char * certFileName,
      const char * keyFileName);
  void SSLCreate(Socket* parent);
  void SSLShowCerts();
  const char* SSLGetCipher();
  const char* SSLAccept();
  int SSLConnect(char * host, int port);
  int SSLConnect(char * host, char * service);
  int SSLRead(void * message, int messageSize);
  int SSLWrite(const void * message, int messageSize);
  int SSLWrite(const void * message);

 private:
  int idSocket;
  int port;
  bool ipv6;
  char type;

  void * SSLContext;	// SSL context
  void * SSLStruct;	// SSL BIO basis input output
};

#endif
