#include "examiner.h"

PENDING(pending, should_fail) {
  ASSERT_EQ("a", "b");
}
