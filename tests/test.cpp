#include "gtest/gtest.h"

const char* hello() {
  return "hello";
}

TEST(IndependentMethod, Hello) {
  EXPECT_EQ("hello", hello());
}
