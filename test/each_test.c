#include "examiner.h"

int __each = 0;

BEFORE_EACH(each) {
  __each += 2;
}

AFTER_EACH(each) {
  __each -= 1;
}

TEST(each, a) {
  ASSERT_EQ(2, __each);
}

TEST(each, b) {
  ASSERT_EQ(3, __each);
}

TEST(each, c) {
  ASSERT_EQ(4, __each);
}
