#include "examiner.h"

TEST(math, test_int, {
  // Obviously this will succeed
  ASSERT_EQUAL(1, 1);
  ASSERT_NOT_EQUAL(1, 2);
});

TEST(math, test_double, {
  // Obviously this will fail
  ASSERT_EQUAL(1.0, 1.0);
  ASSERT_NOT_EQUAL(1.0, 2.0);
});

TEST(math, test_float, {
  // Obviously this will fail
  ASSERT_EQUAL(1.0f, 1.0f);
  ASSERT_NOT_EQUAL(1.0f, 2.0f);
});

TEST(str, test, {
  // This is a super important test
  ASSERT_EQUAL("test", "test");
  ASSERT_NOT_EQUAL("test", "nest");
})

TEST(mem, test, {
  // This is a super important test
  ASSERT_EQUAL_MEM((void *)"Hello, World!", (void *)"Hello, World!", 13);
  ASSERT_NOT_EQUAL_MEM((void *)"abcdefghijklm", (void *)"nopqrstuvwxyz", 13);
})

int main(int argc, char **argv) {
  void *tests[] = {&math_test_int, &math_test_double, &math_test_float,
                   &str_test, &mem_test};
  exam_env env = exam_init(argc, argv, tests);
  return exam_run(&env);
}
