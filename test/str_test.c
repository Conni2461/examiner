#include "examiner.h"

TEST(str, test) {
  ASSERT_EQUAL("test", "test");
  ASSERT_NOT_EQUAL("test", "nest");
}
