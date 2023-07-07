#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

std::string getComputerIp () {
  std::string host;
  host.resize(256);

  gethostname(host.data(), sizeof(host));

  struct hostent* hostentry = gethostbyname(host.data());

  host.clear();
  host = inet_ntoa(*((struct in_addr*) hostentry->h_addr_list[0]));

  return host;
}

void broadcastIsland(
    sockaddr_in& island,
    std::vector<char>& broadcastMessage,
    Socket& broadcastSocket,
    int broadcastIpId,
    std::string& broadcastIpBase
    ) {
  // get the island broadcast ip
  std::string broadcastIp;
  broadcastIp.push_back('-');
  broadcastIp += broadcastIpBase + std::to_string(broadcastIpId);
  (void) island;
  // // set the ip and port for the message to be sent
  // memset(&island, 0, sizeof(sockaddr_in));
  // island.sin_family = AF_INET;
  // island.sin_port = htons(INTERMEDIARY_UDP_PORT); // send to intermediary udp port
  // inet_pton(AF_INET, broadcastIp.data(), &(island.sin_addr));
  broadcastSocket(broadcastIp, INTERMEDIARY_UDP_PORT) << broadcastMessage;

  std::cout << "Broadcasting on: " << broadcastIp.substr(1, broadcastIp.size()) << std::endl;

  // send the broadcast
  //broadcastSocket.sendTo(broadcastMessage.data(), broadcastMessage.size(), &island);
}

void broadCastOnSamePC(std::vector<char>& broadcastMessage, Socket& broadcastSocket) {
  broadcastSocket("", INTERMEDIARY_UDP_PORT) << broadcastMessage;
}

void broadcast(std::vector<char>& message, int code) {
  Socket broadcastSocket('d', false);
  sockaddr_in island;

  // get the computer IP
  std::vector<char> broadcastMessage;

  broadcastMessage.push_back(std::to_string(code)[0]);

  broadcastMessage.push_back(SEPARATOR);

  std::string buffer = getComputerIp();

  for (char character : buffer) {
    broadcastMessage.push_back(character);
  }

  broadcastMessage.push_back(':');

  buffer = std::to_string(PIECES_TCP_PORT).data();

  for (char character : buffer) {
    broadcastMessage.push_back(character);
  }

  broadcastMessage.push_back(SEPARATOR);
  size_t startPosition = broadcastMessage.size();

  broadcastMessage.resize(broadcastMessage.size() + message.size());
  memcpy(&(broadcastMessage.data()[startPosition]), message.data(), message.size());

  // attempt on normal for computer
  broadCastOnSamePC(broadcastMessage, broadcastSocket);

  // get the base for the broadcast IPs
  int broadcastIpId = 15;
  std::string broadcastIpBase = "172.16.123.";

  broadcastSocket.setBroadcast(true);

  // for each vlan
  for (size_t vlan = 200; vlan < 207; vlan++) {
    broadcastIsland(island, broadcastMessage, broadcastSocket, broadcastIpId, broadcastIpBase);

    // set the next broadcast ip
    broadcastIpId += 16;
  }

  std::cout << std::endl;
}

#endif