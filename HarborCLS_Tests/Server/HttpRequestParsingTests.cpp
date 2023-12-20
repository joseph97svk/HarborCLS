//
// Created by josephvalverde on 12/19/23.
//

#include <gtest/gtest.h>

#include "../../HarborCLS_Library/Server/Http/HttpMessages/Request/HttpRequest.hpp"
#include "../../HarborCLS_Library/Server/Http/HttpRequestParser/IRequestParser.hpp"
#include "../../HarborCLS_Library/Server/Socket/TcpSocket.hpp"
#include "Server/Http/HttpRequestParser/HttpRequestParser.hpp"


class HttpParserTests : public ::testing::Test {
protected:
  std::vector<char> request;
  std::shared_ptr<HarborCLS::TcpSocket> socket = nullptr;

  void SetUp() override {
    std::string requestString =
        "GET / HTTP/1.1"
        + std::string("\r\n")
        + "Host: localhost:8080"
        + std::string("\r\n")
        + "Connection: keep-alive"
        + std::string("\r\n")
        + "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"
        + "\r\n\r\n"
        + "Hello World!";

    request = std::vector<char>(requestString.begin(), requestString.end());
  }

  void TearDown() override {
  }
};

TEST_F(HttpParserTests, headerMethodIsParsedCorrectly) {
  std::unique_ptr<HarborCLS::IRequestParser<HarborCLS::TcpSocket, HarborCLS::HttpRequest>> parser =
      std::make_unique<HarborCLS::HttpRequestParser>();

  std::shared_ptr<HarborCLS::HttpRequest> httpRequest = parser->createHttpRequest(request, socket);

  ASSERT_EQ(httpRequest->header.method, HarborCLS::HttpMappings::Method::GET);
}

TEST_F(HttpParserTests, headerPathIsParsedCorrectly) {
  std::unique_ptr<HarborCLS::IRequestParser<HarborCLS::TcpSocket, HarborCLS::HttpRequest>> parser =
      std::make_unique<HarborCLS::HttpRequestParser>();

  std::shared_ptr<HarborCLS::HttpRequest> httpRequest = parser->createHttpRequest(request, socket);

  ASSERT_EQ(httpRequest->header.url, "/");
}

TEST_F(HttpParserTests, headerVersionIsParsedCorrectly) {
  std::unique_ptr<HarborCLS::IRequestParser<HarborCLS::TcpSocket, HarborCLS::HttpRequest>> parser =
      std::make_unique<HarborCLS::HttpRequestParser>();

  std::shared_ptr<HarborCLS::HttpRequest> httpRequest = parser->createHttpRequest(request, socket);

  ASSERT_EQ(httpRequest->header.httpVersion, "HTTP/1.1");
}

TEST_F(HttpParserTests, bodyTypeIsParsedCorrectlyForImages) {
  std::string requestString =
      "GET / HTTP/1.1"
      + std::string("\r\n")
      + "Host: localhost:8080"
      + std::string("\r\n")
      + "Connection: keep-alive"
      + std::string("\r\n")
      + "Accept: image/webp,image/apng,image/*,*/*;q=0.8"
      + "\r\n\r\n"
      + "Hello World!";

  request = std::vector<char>(requestString.begin(), requestString.end());

  std::unique_ptr<HarborCLS::IRequestParser<HarborCLS::TcpSocket, HarborCLS::HttpRequest>> parser =
      std::make_unique<HarborCLS::HttpRequestParser>();

  std::shared_ptr<HarborCLS::HttpRequest> httpRequest = parser->createHttpRequest(request, socket);

  ASSERT_EQ(std::holds_alternative<std::vector<char>>(*httpRequest->body), true);
}

TEST_F(HttpParserTests, bodyTypeIsParsedCorrectly) {
  std::unique_ptr<HarborCLS::IRequestParser<HarborCLS::TcpSocket, HarborCLS::HttpRequest>> parser =
      std::make_unique<HarborCLS::HttpRequestParser>();

  std::shared_ptr<HarborCLS::HttpRequest> httpRequest = parser->createHttpRequest(request, socket);

  ASSERT_EQ(std::holds_alternative<std::string>(*httpRequest->body), true);
}

TEST_F(HttpParserTests, bodyIsParsedCorrectly) {
  std::unique_ptr<HarborCLS::IRequestParser<HarborCLS::TcpSocket, HarborCLS::HttpRequest>> parser =
      std::make_unique<HarborCLS::HttpRequestParser>();

  std::shared_ptr<HarborCLS::HttpRequest> httpRequest = parser->createHttpRequest(request, socket);

  ASSERT_EQ(std::get<std::string>(*httpRequest->body), "Hello World!");
}


