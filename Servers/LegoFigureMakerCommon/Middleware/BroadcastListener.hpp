//
// Created by josephvalverde on 1/12/24.
//

#ifndef HARBOR_CLS_BROADCASTLISTENER_HPP
#define HARBOR_CLS_BROADCASTLISTENER_HPP

#include <HarborCLS.hpp>
#include <utility>
#include "../../../HarborCLS_Library/Server/Middleware/Listener.hpp"
#include "../../../HarborCLS_Library/Server/Socket/UdpSocket.hpp"

class BroadcastListener : public HarborCLS::Listener<std::shared_ptr<std::vector<char>>, HarborCLS::UDPSocket> {
  using ListeningType = std::shared_ptr<std::vector<char>>;

  std::string _ip;
  int _port;

public:
   BroadcastListener(
      HarborCLS::MiddlewareBlockingQueue<ListeningType>& connectionsQueue
      , std::shared_ptr<HarborCLS::UDPSocket> socket
      , unsigned int stopPort
      , std::shared_ptr<HarborCLS::ILogger> logger
      , const std::string ip
      , int port)
      : HarborCLS::Listener<ListeningType , HarborCLS::UDPSocket> (
          connectionsQueue
          , std::move(socket)
          , stopPort
          , std::move(logger))
          , _ip(ip)
          , _port(port) {
      this->_listeningSocket->bind(_port);
   }

   HarborCLS::MiddlewareMessage<ListeningType> obtain() override {
     ListeningType buffer = std::make_shared<std::vector<char>>();

     (*this->_listeningSocket)[_ip, _port] >> *buffer;

     std::string message = std::string(buffer->begin(), buffer->end());

     std::cout << "Received message: " << message << std::endl;

     for (char c : message) {
       std::cout << (int) c << " ";
     }

     std::cout << std::endl;

     return HarborCLS::MiddlewareMessage<ListeningType>(std::move(buffer));
   }
};

#endif //HARBOR_CLS_BROADCASTLISTENER_HPP
