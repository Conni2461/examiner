#include "examiner.h"

TEST(ns1, test_a, {
  // Obviously this will succeed
  ASSERT_EQUAL(1, 1);
});

TEST(ns2, test_a, {
  // Obviously this will fail
  ASSERT_EQUAL(1, 1);
});

TEST(ns2, test_b, {
  // Obviously this will succeed
  ASSERT_EQUAL(1, 1);
});

int main(int argc, char **argv) {
  void *tests[] = {&ns1_test_a, &ns2_test_a, &ns2_test_b};
  exam_env env = exam_init(argc, argv, tests);
  return exam_run(&env);
}
