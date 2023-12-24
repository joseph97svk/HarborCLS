//
// Created by josephvalverde on 12/20/23.
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Logger/LoggerConfiguration.hpp"
#include "Logger/LoggerBuilder/BasicLoggerBuilder.hpp"

class CompletePublicLoggerBuilder : public HarborCLS::BasicLoggerBuilder {
public:
    using HarborCLS::BasicLoggerBuilder::BasicLoggerBuilder;

    HarborCLS::LoggerConfiguration getConfiguration() {
        return _configuration;
    }
};

class LoggerBuilderTests : public ::testing::Test {
protected:
    HarborCLS::LoggerConfiguration _loggerConfiguration{};
    CompletePublicLoggerBuilder _publicVisibilityLoggerBuilder{};
    HarborCLS::BasicLoggerBuilder _loggerBuilder{};

    void SetUp() override {
    }

    void TearDown() override {

    }
};

TEST_F(LoggerBuilderTests, SetConfiguration) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  EXPECT_EQ(_loggerConfiguration, _publicVisibilityLoggerBuilder.getConfiguration());
}

TEST_F(LoggerBuilderTests, Reset) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  _publicVisibilityLoggerBuilder.reset();
  EXPECT_EQ(HarborCLS::LoggerConfiguration(), _publicVisibilityLoggerBuilder.getConfiguration());
}

TEST_F(LoggerBuilderTests, Build) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  EXPECT_NE(nullptr, _publicVisibilityLoggerBuilder.build());
}

TEST_F(LoggerBuilderTests, GetSharedLogger) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  EXPECT_NE(nullptr, _publicVisibilityLoggerBuilder.getSharedLogger());
}

TEST_F(LoggerBuilderTests, SetUniqueSharedLogger) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  _publicVisibilityLoggerBuilder.setUniqueSharedLogger();
  EXPECT_NE(nullptr, _publicVisibilityLoggerBuilder.getSharedLogger());
}

TEST_F(LoggerBuilderTests, GetSharedLoggerReturnsSameInstance) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  std::shared_ptr<HarborCLS::ILogger> logger = _publicVisibilityLoggerBuilder.getSharedLogger();

  std::shared_ptr<HarborCLS::ILogger> logger2 = _publicVisibilityLoggerBuilder.getSharedLogger();

  EXPECT_EQ(logger.get(), logger2.get());
}

TEST_F(LoggerBuilderTests, GetSharedLoggerReturnsSameInstanceAfterReset) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  std::shared_ptr<HarborCLS::ILogger> logger = _publicVisibilityLoggerBuilder.getSharedLogger();

  _publicVisibilityLoggerBuilder.reset();

  std::shared_ptr<HarborCLS::ILogger> logger2 = _publicVisibilityLoggerBuilder.getSharedLogger();

  EXPECT_EQ(logger.get(), logger2.get());
}

TEST_F(LoggerBuilderTests, GetSharedLoggerChangesInstanceAfterSetUniqueSharedLogger) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  std::shared_ptr<HarborCLS::ILogger> logger = _publicVisibilityLoggerBuilder.getSharedLogger();

  _publicVisibilityLoggerBuilder.setUniqueSharedLogger();

  std::shared_ptr<HarborCLS::ILogger> logger2 = _publicVisibilityLoggerBuilder.getSharedLogger();

  EXPECT_NE(logger.get(), logger2.get());
}

TEST_F(LoggerBuilderTests, MultipleBuildsReturnsSuccessfully) {
  _publicVisibilityLoggerBuilder.setConfiguration(_loggerConfiguration);
  std::shared_ptr<HarborCLS::ILogger> logger = _publicVisibilityLoggerBuilder.getSharedLogger();
  std::shared_ptr<HarborCLS::ILogger> logger2 = _publicVisibilityLoggerBuilder.getSharedLogger();
  std::shared_ptr<HarborCLS::ILogger> logger3 = _publicVisibilityLoggerBuilder.getSharedLogger();

  EXPECT_NE(nullptr, logger);
  EXPECT_NE(nullptr, logger2);
  EXPECT_NE(nullptr, logger3);
}
