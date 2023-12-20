//
// Created by josephvalverde on 12/20/23.
//

#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>
#include "Logger/LogFileRotationPolicies/NoLogFileRotation.hpp"

class NoLogFileRotationTest : public ::testing::Test {
protected:
  std::string testFileName = "test.log";
  std::ofstream testFile;

  void SetUp() override {
  }

  void TearDown() override {
    if (testFile.is_open()) {
      testFile.close();
    }
    std::remove(testFileName.c_str());
  }
};

TEST_F(NoLogFileRotationTest, OpenFileIfNotOpen) {
  HarborCLS::NoLogFileRotation rotation;

  ASSERT_FALSE(testFile.is_open());

  rotation.rotateLogFile(testFile, testFileName);
  ASSERT_TRUE(testFile.is_open());
}

TEST_F(NoLogFileRotationTest, NoRotationWhenFileOpen) {
  HarborCLS::NoLogFileRotation rotation;

  testFile.open(testFileName, std::ios::app);
  std::string initialFileName = testFileName;

  for (int i = 0; i < 5; ++i) {
    rotation.rotateLogFile(testFile, testFileName);
  }

  ASSERT_TRUE(testFile.is_open());
  ASSERT_EQ(testFileName, initialFileName);
}
