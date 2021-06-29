#include "examiner.h"

int __each = 0;

BEFORE_EACH(each) {
  __each += 2;
}

AFTER_EACH(each) {
  __each -= 1;
}

TEST(each, a) {
  ASSERT_EQUAL(2, __each);
}

TEST(each, b) {
  ASSERT_EQUAL(3, __each);
}

TEST(each, c) {
  ASSERT_EQUAL(4, __each);
}
