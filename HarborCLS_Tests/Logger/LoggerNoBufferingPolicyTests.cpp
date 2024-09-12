//
// Created by josephvalverde on 12/20/23.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Logger/LoggingBufferingPolicies/NoBufferingPolicy.hpp"

TEST(NoBufferingPolicyTests, Buffer) {
  HarborCLS::NoBufferingPolicy noBufferingPolicy;

  std::string message = "Hello, World!";

  std::optional<std::string> result = noBufferingPolicy.buffer(message);
  ASSERT_EQ(result, message);
}