//
// Created by josephvalverde on 12/20/23.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Logger/LoggingFileManagementPolicies/FileOpenOnDemandPolicy.hpp"

class MockFileRotation : public HarborCLS::ILogFileRotation {
public:
  MOCK_METHOD(void, rotateLogFile, (std::ofstream &fileStream, std::string &logFilePath), (override));
};

class FileOpenOnDemandPolicyTest : public ::testing::Test {
public:
  std::string testFileName = "test.log";

  std::unique_ptr<MockFileRotation> mockFileRotation;

  std::optional<std::reference_wrapper<std::ofstream>> fileRefCapture;

protected:

  void SetUp() override {
    mockFileRotation = std::make_unique<MockFileRotation>();
  }

  void TearDown() override {

  }
};

TEST_F(FileOpenOnDemandPolicyTest, GetLogFileReturnsLogFile) {
  EXPECT_CALL(*mockFileRotation, rotateLogFile)
      .Times(0);

  std::string originalFileName = testFileName;

  HarborCLS::FileOpenOnDemandPolicy rotation(std::move(mockFileRotation));

  std::variant<std::string, std::ofstream> file = rotation.getLogFile(testFileName);

  ASSERT_TRUE(std::holds_alternative<std::string>(file));

  std::string result = std::move(std::get<std::string>(file));

  ASSERT_TRUE(testFileName.empty());
  ASSERT_TRUE(result == originalFileName);
}

TEST_F(FileOpenOnDemandPolicyTest, LoggingLeavesFileClosedAfterwards) {
  EXPECT_CALL(*mockFileRotation, rotateLogFile)
      .Times(1)
      .WillOnce([this](std::ofstream &file, std::string &fileName) {
        file.open(fileName, std::ios::out | std::ios::trunc);
        this->fileRefCapture = std::optional<std::reference_wrapper<std::ofstream>>(file);
      });

  HarborCLS::FileOpenOnDemandPolicy rotation(std::move(mockFileRotation));

  std::variant<std::string, std::ofstream> file = rotation.getLogFile(testFileName);

  std::string result = std::move(std::get<std::string>(file));

  std::optional<std::reference_wrapper<std::ofstream>> fileRef = std::nullopt;
  std::mutex canWrite;

  rotation.log("test", fileRef, canWrite);

  ASSERT_TRUE(fileRefCapture.has_value());
  ASSERT_FALSE(fileRefCapture.value().get().is_open());
}

