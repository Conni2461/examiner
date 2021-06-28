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
