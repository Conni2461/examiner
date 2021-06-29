#include "examiner.h"

PENDING(pending, should_fail) {
  ASSERT_EQUAL("a", "b");
}
