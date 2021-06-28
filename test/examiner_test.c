#include "examiner.h"

#include <stdio.h>

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

TEST(boolean, should_work) {
  ASSERT_TRUE(true);
  ASSERT_FALSE(false);
}

TEST(str, test) {
  ASSERT_EQUAL("test", "test");
  ASSERT_NOT_EQUAL("test", "nest");
}

TEST(mem, test) {
  ASSERT_EQUAL_MEM((void *)"Hello, World!", (void *)"Hello, World!", 13);
  ASSERT_NOT_EQUAL_MEM((void *)"abcdefghijklm", (void *)"nopqrstuvwxyz", 13);
}

PENDING(pending, should_fail) {
  ASSERT_EQUAL("a", "b");
}

int main(int argc, char **argv) {
  exam_init(argc, argv);
  return exam_run();
}
