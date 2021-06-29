#include "examiner.h"

TEST(mem, test) {
  ASSERT_EQUAL_MEM((void *)"Hello, World!", (void *)"Hello, World!", 13);
  ASSERT_NOT_EQUAL_MEM((void *)"abcdefghijklm", (void *)"nopqrstuvwxyz", 13);
}
