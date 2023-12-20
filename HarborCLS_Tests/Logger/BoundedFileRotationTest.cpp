//
// Created by josephvalverde on 12/20/23.
//

#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "../../HarborCLS_Library/Logger/LogFileRotationPolicies/BoundedFileRotation.hpp"

class BoundedFileRotationTest : public ::testing::Test {
protected:
  std::string testFileName = "test.log";
  std::ofstream testFile;

  void SetUp() override {
    testFile.open(testFileName, std::ios::out | std::ios::trunc);
  }

  void TearDown() override {
    testFile.close();
    std::remove(testFileName.c_str());
  }
};

TEST_F(BoundedFileRotationTest, RotateLogFile) {
  unsigned int maxWrites = 5;
  HarborCLS::BoundedFileRotation rotation(maxWrites);

  for (unsigned int i = 0; i < maxWrites; ++i) {
    rotation.rotateLogFile(testFile, testFileName);
    ASSERT_TRUE(testFile.is_open());
  }

  rotation.rotateLogFile(testFile, testFileName);
  ASSERT_TRUE(testFile.is_open());
}

TEST_F(BoundedFileRotationTest, OpenFileIfNotOpen) {
  HarborCLS::BoundedFileRotation rotation(10);

  testFile.close();
  rotation.rotateLogFile(testFile, testFileName);

  ASSERT_TRUE(testFile.is_open());
}


