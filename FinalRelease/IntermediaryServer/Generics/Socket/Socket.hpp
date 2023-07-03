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

#ifndef SOCKET_H
#define SOCKET_H

#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

class Socket
{

public:
  Socket(char type, bool IPv6 = false); // char == 's' => stream, char == 'd' => datagram
  Socket(int socketFD);                 // int = socket descriptor
  ~Socket();
  int Connect(const char *host, int port);
  int Connect(const char *host, const char *service);
  void Close();
  int Read(void *buffer, int bufferSize);
  int Write(const void *buffer, int bufferSize);
  int Write(const char *buffer); // size is calculated with strlen
  int Listen(int backlog);
  int Bind(int port);
  // Socket * Accept();
  std::shared_ptr<Socket> Accept();
  int Shutdown(int mode); // mode = { SHUT_RD, SHUT_WR, SHUT_RDWR }
  void SetIDSocket(int newId);
  int sendTo(void *message, int messageSize, void *socketAddress);
  int recvFrom(void *message, int messageSize, void *socketAddress);

  void InitSSL();
  void SSLInit()
  {
    InitSSL();
  }
  void InitSSLContext();
  void SSLInitServerContext();
  void SSLInitServer(
      const char *certificateFileName,
      const char *keyFileName);
  void SSLLoadCertificates(const char *certFileName,
                           const char *keyFileName);
  void SSLCreate(Socket *parent);
  void SSLShowCerts();
  const char *SSLGetCipher();
  const char *SSLAccept();
  int SSLConnect(char *host, int port);
  int SSLConnect(char *host, char *service);
  int SSLRead(void *message, int messageSize);
  int SSLWrite(const void *message, int messageSize);
  int SSLWrite(const void *message);

  bool isSSL();
  void setBufferDefault(int size);

  Socket &operator<<(std::string &text)
  {
    int bytesWriten = 0;

    if (this->isSSL())
    {
      bytesWriten = this->SSLWrite(text.data());
    }
    else
    {
      bytesWriten = this->Write(text.data());
    }

    this->bytesReadWritten = bytesWriten;

    return *this;
  }

  template <typename dataType>
  Socket &operator<<(dataType data)
  {
    int bytesWriten = 0;

    if (this->isSSL())
    {
      bytesWriten = this->SSLWrite(data, sizeof(dataType));
    }
    else
    {
      bytesWriten = this->Write(data, sizeof(dataType));
    }

    this->bytesReadWritten = bytesWriten;

    return *this;
  }

  template <typename dataType>
  Socket &operator<<(std::vector<dataType> data)
  {
    int bytesWriten = 0;

    if (this->isSSL())
    {
      bytesWriten = this->SSLWrite((void *)data.data(),
                                   (int)data.size() * sizeof(dataType));
    }
    else
    {
      bytesWriten = this->Write((void *)data.data(),
                                (int)data.size() * sizeof(dataType));
    }

    this->bytesReadWritten = bytesWriten;

    return *this;
  }

  Socket &operator>>(std::string &text)
  {
    char buffer[this->bufferDefaultSize];
    memset(buffer, 0, this->bufferDefaultSize);
    int bytesRead = 0;

    if (this->isSSL())
    {
      bytesRead = this->SSLRead(buffer, this->bufferDefaultSize);
    }
    else
    {
      bytesRead = this->Read(buffer, this->bufferDefaultSize);
    }

    this->bytesReadWritten = bytesRead;

    text.resize(bytesRead + 1);
    text = buffer;

    return *this;
  }

  template <typename dataType>
  Socket &operator>>(dataType &data)
  {
    int bytesRead = 0;

    if (this->isSSL())
    {
      bytesRead = this->SSLRead(data, sizeof(dataType));
    }
    else
    {
      bytesRead = this->Read(data, sizeof(dataType));
    }

    this->bytesReadWritten = bytesRead;

    return *this;
  }

  template <typename dataType>
  Socket &operator>>(std::vector<dataType> &data)
  {
    char buffer[this->bufferDefaultSize];
    memset(buffer, 0, this->bufferDefaultSize);
    int bytesRead = 0;
    if (this->isSSL())
    {
      bytesRead = this->SSLRead(buffer, this->bufferDefaultSize);
    }
    else
    {
      bytesRead = this->Read(buffer, this->bufferDefaultSize);
    }

    int elementsRead = bytesRead / sizeof(dataType);

    data.resize(elementsRead);
    memcpy(data.data(), buffer, bytesRead);

    this->bytesReadWritten = bytesRead;

    return *this;
  }

  operator int() const {
    return this->bytesReadWritten;
  }

  void increaseTimeout(size_t time) {
    struct timeval timeout;
    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    if (setsockopt(this->idSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof timeout) < 0)
    {
      perror("RCV: setsockopt failed\n");
    }
 
    if (setsockopt(this->idSocket, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                   sizeof timeout) < 0)
    {
       perror("SND: setsockopt failed\n");
    }
  }

  void setBroadcast(bool activateBroadcast) {
    int broadcastEnable = activateBroadcast ? 1 : 0;

    if (setsockopt(
        this->idSocket,
        SOL_SOCKET,
        SO_BROADCAST,
        &broadcastEnable,
        sizeof(broadcastEnable))
        < 0) {
      perror("Broadcast enable error!\n");
    }
  }

private:
  int idSocket;
  int port;
  bool ipv6;
  char type;

  void *SSLContext; // SSL context
  void *SSLStruct;  // SSL BIO basis input output

  int bufferDefaultSize = 500;

  int bytesReadWritten = 0;
};

#endif
