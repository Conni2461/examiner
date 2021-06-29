#include "examiner.h"

TEST(str, char) {
  ASSERT_EQUAL((char)'t', (char)'t');
  ASSERT_NOT_EQUAL((char)'t', (char)'f');
}

TEST(str, array) {
  ASSERT_EQUAL("test", "test");
  ASSERT_NOT_EQUAL("test", "nest");
}
