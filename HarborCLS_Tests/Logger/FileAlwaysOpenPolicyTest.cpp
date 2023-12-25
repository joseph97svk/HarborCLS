//
// Created by josephvalverde on 12/20/23.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Logger/LoggingFileManagementPolicies/FileAlwaysOpenPolicy.hpp"

class MockFileRotation : public HarborCLS::ILogFileRotation {
public:
  MOCK_METHOD(void, rotateLogFile, (std::ofstream &fileStream, std::string &logFilePath), (override));
};

class FileAlwaysOpenPolicyTest : public ::testing::Test {
public:
  std::string testFileName = "test.log";

  std::unique_ptr<MockFileRotation> mockFileRotation;

  std::ofstream testFile;
protected:

  void SetUp() override {
    mockFileRotation = std::make_unique<MockFileRotation>();
  }

  void TearDown() override {
    if (testFile.is_open()) {
      testFile.close();
    }
  }
};

TEST_F(FileAlwaysOpenPolicyTest, OpenFileIfNotOpenOnGetLogFile) {
  if (testFile.is_open()) {
    testFile.close();
  }

  EXPECT_CALL(*mockFileRotation, rotateLogFile)
      .Times(1)
      .WillOnce([](std::ofstream &file, std::string &fileName) {
        file.open(fileName, std::ios::out | std::ios::trunc);
      });

  HarborCLS::FileAlwaysOpenPolicy rotation(std::move(mockFileRotation));

  std::variant<std::string, std::ofstream> file = rotation.getLogFile(testFileName);

  ASSERT_TRUE(std::holds_alternative<std::ofstream>(file));

  testFile = std::move(std::get<std::ofstream>(file));

  ASSERT_TRUE(testFile.is_open());
}
