#include "examiner.h"

TEST(mem, test) {
  ASSERT_EQ_MEM((void *)"Hello, World!", (void *)"Hello, World!", 13);
  ASSERT_NE_MEM((void *)"abcdefghijklm", (void *)"nopqrstuvwxyz", 13);
}

TEST(mem, ptr) {
  int a = 5;
  int b = a;

  ASSERT_EQ((void *)&a, (void *)&a);
  ASSERT_NE((void *)&a, (void *)&b);
}
