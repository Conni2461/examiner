#include "examiner.h"

TEST(math, test_int) {
  ASSERT_EQUAL(1, 1);
  ASSERT_NOT_EQUAL(1, 2);
}

TEST(math, test_unsigned) {
  ASSERT_EQUAL(1U, 1U);
  ASSERT_NOT_EQUAL(1U, 2U);
}

TEST(math, test_long) {
  ASSERT_EQUAL(1L, 1L);
  ASSERT_NOT_EQUAL(1L, 2L);
}

TEST(math, test_unsigned_long) {
  ASSERT_EQUAL(1UL, 1UL);
  ASSERT_NOT_EQUAL(1UL, 2UL);
}

TEST(math, test_long_long) {
  ASSERT_EQUAL(1LL, 1LL);
  ASSERT_NOT_EQUAL(1LL, 2LL);
}

TEST(math, test_unsigned_long_long) {
  ASSERT_EQUAL(1ULL, 1ULL);
  ASSERT_NOT_EQUAL(1ULL, 2ULL);
}

TEST(math, test_double) {
  ASSERT_EQUAL(1.0, 1.0);
  ASSERT_NOT_EQUAL(1.0, 2.0);
}

TEST(math, test_float) {
  ASSERT_EQUAL(1.0f, 1.0f);
  ASSERT_NOT_EQUAL(1.0f, 2.0f);
}
