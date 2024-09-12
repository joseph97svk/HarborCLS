#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <filesystem>
#include <vector>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define NO_COPY(class) \
  class(const class&) = delete; \
  class& operator=(const class&) = delete; \
  class(class&&) = delete; \
  class& operator=(class&&) = delete;

namespace HarborCLS {

  [[nodiscard]] inline static std::string getComputerIp() {
    std::string host;
    host.resize(256);

    gethostname(host.data(), host.size());

    struct hostent *hostEntry = gethostbyname(host.data());

    host.clear();
    host = inet_ntoa(*((struct in_addr *) hostEntry->h_addr_list[0]));

    return host;
  }

  [[nodiscard]] inline static std::vector<char> transformStringIntoCharVector(std::string& string) noexcept {
    std::vector<char> vector (string.begin(), string.end());

    return vector;
  }
}
#endif
