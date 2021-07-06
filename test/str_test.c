#include "examiner.h"

TEST(str, char) {
  ASSERT_EQ((char)'t', (char)'t');
  ASSERT_NE((char)'t', (char)'f');
}

TEST(str, array) {
  ASSERT_EQ("test", "test");
  ASSERT_NE("test", "nest");
}
