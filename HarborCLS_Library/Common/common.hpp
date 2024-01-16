#ifndef COMMON_HPP
#define COMMON_HPP

#include <memory>
#include <string>
#include <netdb.h>
#include <filesystem>

#include <sys/socket.h>
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

  [[nodiscard]] inline static std::string getCurrentTimeAndDate() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%F.%T");

    return ss.str();
  }

  [[nodiscard]] inline static std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%T");

    return ss.str();
  }

  [[nodiscard]] inline static std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%F");

    return ss.str();
  }

  [[nodiscard]] inline static std::vector<char> transformStringIntoCharVector(std::string& string) noexcept {
    std::vector <char> vector (string.begin(), string.end());

    return vector;
  }
}
#endif
