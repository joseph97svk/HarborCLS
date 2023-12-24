//
// Created by josephvalverde on 12/19/23.
//

#include <gtest/gtest.h>

#include "../../HarborCLS_Library/Server/Http/ResponseHeaderComposer/HttpResponseHeaderComposer.hpp"

class HttpComposerTests : public ::testing::Test {
protected:
  HarborCLS::HttpResponseHeaderComposer composer;

  std::shared_ptr<HarborCLS::HttpResponse> response;

  void SetUp() override {
    response = std::make_shared<HarborCLS::HttpResponse>();
    response->htmlVersion = "HTTP/1.1";
    response->statusCode = HarborCLS::StatusCode::OK;
    response->contentType = HarborCLS::ContentType::TextHtml;
    response->body = std::string("Hello World!");
    response->contentLength = std::get<std::string>(response->body).length();
  }

  void TearDown() override {
  }
};

TEST_F(HttpComposerTests, headerIsComposedCorrectly) {
  std::string header = composer.composeHeader(response);

  std::string expectedHeader =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 12\r\n"
      "\r\n\r\n";

  ASSERT_EQ(header, expectedHeader);
}

TEST_F(HttpComposerTests, headerWithAdditionalFieldsIsComposedCorrectly) {
  response->otherHeaderFields.push_back({"Test", "TestValue"});

  std::string header = composer.composeHeader(response);

  std::string expectedHeader =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/html\r\n"
      "Content-Length: 12\r\n"
      "Test: TestValue\r\n"
      "\r\n\r\n";

  ASSERT_EQ(header, expectedHeader);
}

