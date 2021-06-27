#include "examiner.h"

TEST(math, test_int) {
  // Obviously this will succeed
  ASSERT_EQUAL(1, 1);
  ASSERT_NOT_EQUAL(1, 2);
}

TEST(math, test_double) {
  // Obviously this will fail
  ASSERT_EQUAL(1.0, 1.0);
  ASSERT_NOT_EQUAL(1.0, 2.0);
}

TEST(math, test_float) {
  // Obviously this will fail
  ASSERT_EQUAL(1.0f, 1.0f);
  ASSERT_NOT_EQUAL(1.0f, 2.0f);
}

TEST(str, test) {
  // This is a super important test
  ASSERT_EQUAL("test", "test");
  ASSERT_NOT_EQUAL("test", "nest");
}

TEST(mem, test) {
  // This is a super important test
  ASSERT_EQUAL_MEM((void *)"Hello, World!", (void *)"Hello, World!", 13);
  ASSERT_NOT_EQUAL_MEM((void *)"abcdefghijklm", (void *)"nopqrstuvwxyz", 13);
}

PENDING(pending, should_fail) {
  ASSERT_EQUAL("a", "b");
}

int main(int argc, char **argv) {
  exam_env env = exam_init(argc, argv);
  return exam_run(&env);
}
