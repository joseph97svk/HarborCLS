//
// Created by josephvalverde on 12/20/23.
//
#include <utility>
#include <variant>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../../HarborCLS_Library/Logger/Logger.hpp"
#include "../../HarborCLS_Library/Logger/LoggingFileManagementPolicies/ILoggerFileManagementPolicy.hpp"

class LoggerFileManagementMock : public HarborCLS::ILoggerFileManagementPolicy {
public:
    std::string localFilePath;

    std::string loggingMessageSent;

    MOCK_METHOD((std::variant<std::string, std::ofstream>), getLogFile, (std::string &logFilePath), (override));

    MOCK_METHOD(void, log, (std::string completeLoggingMessage,
                         std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
                    std::mutex &canWrite), (override)) ;
};

class LoggerBufferingMock : public HarborCLS::ILoggerBufferingPolicy {
public:
    MOCK_METHOD((std::optional<std::string>), buffer, (std::string &message), (override));
};

class LoggerFileRotationMock : public HarborCLS::ILogFileRotation {
public:
    MOCK_METHOD(void, rotateLogFile, (std::ofstream &file, std::string &fileName), (override));
};

class LoggerTests : public ::testing::Test {
protected:
  LoggerFileManagementMock *_fileManagementMock;
  LoggerBufferingMock *_bufferingMock;
  LoggerFileRotationMock *_fileRotationMock;

  std::string _logFilePath = "test.log";

  void SetUp() override {
    _fileManagementMock = new LoggerFileManagementMock();
    _bufferingMock = new LoggerBufferingMock();
    _fileRotationMock = new LoggerFileRotationMock();
  }

  void TearDown() override {

  }

};

TEST_F(LoggerTests, constructorCallsFileManagementGetLogFile) {
  std::string logFilePath = "test.log";

  EXPECT_CALL(*_fileManagementMock, getLogFile)
      .Times(1)
      .WillOnce([this](std::string &logFilePath) {
        std::ofstream file(logFilePath, std::ios::app);

        this->_fileManagementMock->localFilePath = logFilePath;

        return file;
      });

  EXPECT_CALL(*_fileManagementMock, log)
      .Times(0);

  EXPECT_CALL(*_bufferingMock, buffer)
      .Times(0);

  EXPECT_CALL(*_fileRotationMock, rotateLogFile)
      .Times(0);

  HarborCLS::Logger logger(std::shared_ptr<HarborCLS::ILoggerBufferingPolicy>(_bufferingMock),
                           std::shared_ptr<HarborCLS::ILoggerFileManagementPolicy>(_fileManagementMock),
                           std::shared_ptr<HarborCLS::ILogFileRotation>(_fileRotationMock),
                           logFilePath);

  ASSERT_EQ(_fileManagementMock->localFilePath, logFilePath);
}

TEST_F(LoggerTests, logCallsFileManagementLog) {
  EXPECT_CALL(*_fileManagementMock, getLogFile)
      .Times(1)
      .WillOnce(testing::Return(std::ofstream()));

  EXPECT_CALL(*_fileManagementMock, log)
      .Times(1)
      .WillOnce([this](std::string completeLoggingMessage,
                       std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
                       std::mutex &canWrite) {
        this->_fileManagementMock->loggingMessageSent = std::move(completeLoggingMessage);
        return;
      });

  EXPECT_CALL(*_bufferingMock, buffer)
      .Times(1)
      .WillOnce([](std::string &message) {
        return message;
      });

  EXPECT_CALL(*_fileRotationMock, rotateLogFile)
      .Times(0);

  HarborCLS::Logger logger(std::shared_ptr<HarborCLS::ILoggerBufferingPolicy>(_bufferingMock),
                           std::shared_ptr<HarborCLS::ILoggerFileManagementPolicy>(_fileManagementMock),
                           std::shared_ptr<HarborCLS::ILogFileRotation>(_fileRotationMock),
                           _logFilePath);

  logger.log("testing message");

  // since all write operations are async then this wait is necessary
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  ASSERT_EQ(_fileManagementMock->loggingMessageSent.contains(" [ Log ] : testing message"), true);
}

TEST_F(LoggerTests, logCallsFileManagementWithoutWaitFails) {
  EXPECT_CALL(*_fileManagementMock, getLogFile)
      .Times(1)
      .WillOnce(testing::Return(std::ofstream()));

  EXPECT_CALL(*_fileManagementMock, log)
      .Times(1)
      .WillOnce([this](std::string completeLoggingMessage,
                       std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
                       std::mutex &canWrite) {
        this->_fileManagementMock->loggingMessageSent = std::move(completeLoggingMessage);
        return;
      });

  EXPECT_CALL(*_bufferingMock, buffer)
      .Times(1)
      .WillOnce([](std::string &message) {
        return message;
      });

  EXPECT_CALL(*_fileRotationMock, rotateLogFile)
      .Times(0);

  HarborCLS::Logger logger(std::shared_ptr<HarborCLS::ILoggerBufferingPolicy>(_bufferingMock),
                           std::shared_ptr<HarborCLS::ILoggerFileManagementPolicy>(_fileManagementMock),
                           std::shared_ptr<HarborCLS::ILogFileRotation>(_fileRotationMock),
                           _logFilePath);

  logger.log("testing message");

  ASSERT_EQ(_fileManagementMock->loggingMessageSent.empty(), true);
}

TEST_F(LoggerTests, logCallsFileManagementLogWithBuffering) {
  EXPECT_CALL(*_fileManagementMock, getLogFile)
      .Times(1)
      .WillOnce(testing::Return(std::ofstream()));

  EXPECT_CALL(*_fileManagementMock, log)
      .Times(0);

  EXPECT_CALL(*_bufferingMock, buffer)
      .Times(1)
      .WillOnce([](std::string &message) {
        return std::nullopt;
      });

  EXPECT_CALL(*_fileRotationMock, rotateLogFile)
      .Times(0);

  HarborCLS::Logger logger(std::shared_ptr<HarborCLS::ILoggerBufferingPolicy>(_bufferingMock),
                           std::shared_ptr<HarborCLS::ILoggerFileManagementPolicy>(_fileManagementMock),
                           std::shared_ptr<HarborCLS::ILogFileRotation>(_fileRotationMock),
                           _logFilePath);

  logger.log("testing message");

  // since all write operations are async then this wait is necessary
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  ASSERT_EQ(_fileManagementMock->loggingMessageSent.empty(), true);
}

TEST_F(LoggerTests, warningCallLogsMessage) {
  EXPECT_CALL(*_fileManagementMock, getLogFile)
      .Times(1)
      .WillOnce(testing::Return(std::ofstream()));

  EXPECT_CALL(*_fileManagementMock, log)
      .Times(1)
      .WillOnce([this](std::string completeLoggingMessage,
                       std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
                       std::mutex &canWrite) {
        this->_fileManagementMock->loggingMessageSent = std::move(completeLoggingMessage);
        return;
      });

  EXPECT_CALL(*_bufferingMock, buffer)
      .Times(1)
      .WillOnce([](std::string &message) {
        return message;
      });

  EXPECT_CALL(*_fileRotationMock, rotateLogFile)
      .Times(0);

  HarborCLS::Logger logger(std::shared_ptr<HarborCLS::ILoggerBufferingPolicy>(_bufferingMock),
                           std::shared_ptr<HarborCLS::ILoggerFileManagementPolicy>(_fileManagementMock),
                           std::shared_ptr<HarborCLS::ILogFileRotation>(_fileRotationMock),
                           _logFilePath);

  logger.warning("testing message");

  // since all write operations are async then this wait is necessary
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  ASSERT_EQ(_fileManagementMock->loggingMessageSent.contains(" [ Warning ] : testing message"), true);
}

TEST_F(LoggerTests, errorCallLogsMessage) {
  EXPECT_CALL(*_fileManagementMock, getLogFile)
      .Times(1)
      .WillOnce(testing::Return(std::ofstream()));

  EXPECT_CALL(*_fileManagementMock, log)
      .Times(1)
      .WillOnce([this](std::string completeLoggingMessage,
                       std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
                       std::mutex &canWrite) {
        this->_fileManagementMock->loggingMessageSent = std::move(completeLoggingMessage);
        return;
      });

  EXPECT_CALL(*_bufferingMock, buffer)
      .Times(1)
      .WillOnce([](std::string &message) {
        return message;
      });

  EXPECT_CALL(*_fileRotationMock, rotateLogFile)
      .Times(0);

  HarborCLS::Logger logger(std::shared_ptr<HarborCLS::ILoggerBufferingPolicy>(_bufferingMock),
                           std::shared_ptr<HarborCLS::ILoggerFileManagementPolicy>(_fileManagementMock),
                           std::shared_ptr<HarborCLS::ILogFileRotation>(_fileRotationMock),
                           _logFilePath);

  logger.error("testing message");

  // since all write operations are async then this wait is necessary
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  ASSERT_EQ(_fileManagementMock->loggingMessageSent.contains(" [ Error ] : testing message"), true);
}

TEST_F(LoggerTests, infoCallLogsMessage) {
  EXPECT_CALL(*_fileManagementMock, getLogFile)
      .Times(1)
      .WillOnce(testing::Return(std::ofstream()));

  EXPECT_CALL(*_fileManagementMock, log)
      .Times(1)
      .WillOnce([this](std::string completeLoggingMessage,
                       std::optional<std::reference_wrapper<std::ofstream>> &loggingFile,
                       std::mutex &canWrite) {
        this->_fileManagementMock->loggingMessageSent = std::move(completeLoggingMessage);
        return;
      });

  EXPECT_CALL(*_bufferingMock, buffer)
      .Times(1)
      .WillOnce([](std::string &message) {
        return message;
      });

  EXPECT_CALL(*_fileRotationMock, rotateLogFile)
      .Times(0);

  HarborCLS::Logger logger(std::shared_ptr<HarborCLS::ILoggerBufferingPolicy>(_bufferingMock),
                           std::shared_ptr<HarborCLS::ILoggerFileManagementPolicy>(_fileManagementMock),
                           std::shared_ptr<HarborCLS::ILogFileRotation>(_fileRotationMock),
                           _logFilePath);

  logger.info("testing message");

  // since all write operations are async then this wait is necessary
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  ASSERT_EQ(_fileManagementMock->loggingMessageSent.contains(" [ Info ] : testing message"), true);
}
