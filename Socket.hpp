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
        Socket( char, bool IPv6 = false );	// char == 's' => stream, char == 'd' => datagram
        Socket( int );				// int = socket descriptor
        ~Socket();
        int Connect( const char * host, int port );
        int Connect( const char * host, const char * service );
        void Close();
        int Read( void * buffer, size_t bufferSize );
        int Write( const void * buffer, size_t bufferSize );
        int Write( const char * buffer );	// size is calculated with strlen
        int Listen( int backlog );
        int Bind( int port );
        Socket * Accept();
        int Shutdown( int mode );		// mode = { SHUT_RD, SHUT_WR, SHUT_RDWR }
        void SetIDSocket( int newId );
        int sendTo( const void * buffer, size_t bufferSize, void * receptor );
        int recvFrom(void * buffer, size_t bufferSize, void * receptor);
        

        void SSLInitContext();
        void SSLInit();
        void SSLInitServerContext();
        void SSLInitServer( const char *, const char * );
        void SSLCreate( Socket * parent ); //[assign a new SSL using same context as parent]
        void SSLShowCerts(); //[SSL_get_peer_certificate()]
        const char * SSLGetCipher(); //[SSL_get_cypher()]
        const char * SSLAccept(); //[SSL_accept()];
        int SSLConnect( char *, int );
        int SSLConnect( char *, char * );
        int SSLRead( void *, int );
        int SSLWrite( void *, int );
        void SSLLoadCertificates( const char * certFileName, const char * keyFileName );

    private:
        int idSocket;
        int port;
        bool ipv6;

        void * SSLContext;
	    void * SSLStruct;
};

#endif

