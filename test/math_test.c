#include "examiner.h"

TEST(math, test_int) {
  ASSERT_EQUAL(1, 1);
  ASSERT_NOT_EQUAL(1, 2);
}

TEST(math, test_double) {
  ASSERT_EQUAL(1.0, 1.0);
  ASSERT_NOT_EQUAL(1.0, 2.0);
}

TEST(math, test_float) {
  ASSERT_EQUAL(1.0f, 1.0f);
  ASSERT_NOT_EQUAL(1.0f, 2.0f);
}
