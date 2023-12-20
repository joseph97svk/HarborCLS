//
// Created by josephvalverde on 12/19/23.
//

#include <gtest/gtest.h>

TEST(SampleTest, AssertionTrue) {
  ASSERT_TRUE(true);  // Example test
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
