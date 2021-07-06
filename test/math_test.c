#include "examiner.h"

TEST(math, test_int) {
  ASSERT_EQ(1, 1);
  ASSERT_NE(1, 2);
}

TEST(math, test_unsigned) {
  ASSERT_EQ(1U, 1U);
  ASSERT_NE(1U, 2U);
}

TEST(math, test_long) {
  ASSERT_EQ(1L, 1L);
  ASSERT_NE(1L, 2L);
}

TEST(math, test_unsigned_long) {
  ASSERT_EQ(1UL, 1UL);
  ASSERT_NE(1UL, 2UL);
}

TEST(math, test_long_long) {
  ASSERT_EQ(1LL, 1LL);
  ASSERT_NE(1LL, 2LL);
}

TEST(math, test_unsigned_long_long) {
  ASSERT_EQ(1ULL, 1ULL);
  ASSERT_NE(1ULL, 2ULL);
}

TEST(math, test_double) {
  ASSERT_EQ(1.0, 1.0);
  ASSERT_NE(1.0, 2.0);
}

TEST(math, test_float) {
  ASSERT_EQ(1.0f, 1.0f);
  ASSERT_NE(1.0f, 2.0f);
}
