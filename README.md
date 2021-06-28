# examiner

A small, opinionated c unit testing framework.

## Build

```bash
make -j TYPE=-O2    # or a different optimization, like -Ofast. Default is: `-Og --ggdb3` (might change soon)
sudo make install   # to install it
sudo make uninstall # to remove it again
```

## Documentation

```c
#include "examiner.h"

/** Define a new test with a scope and a name. The resulting test name will then
 *  always be: scope + "." + name
 *  This name can be used to filter on command line later, and only run specific
 *  tests.
 */
TEST(math, test_int) {
  /** ASSERT_EQUAL and ASSERT_NOT_EQUAL are two macros that will dispatch based
   *  on the type to internal comparator functions. It dispatches
   *  `int`, `float`, `double`, `char *`
   */
  ASSERT_EQUAL(1, 1);
  ASSERT_NOT_EQUAL(1, 2);
}

TEST(str, test) {
  ASSERT_EQUAL("test", "test");
  ASSERT_NOT_EQUAL("test", "nest");
}

/**
 * PENDING means that the feature the test is testing is probably not
 * implemented yet so the test will not be executed.
 */
PENDING(pending, should_fail) {
  ASSERT_EQUAL("a", "b");
}

/**
 * Also awailable are:
 * ASSERT_EQUAL_MEM
 * ASSERT_NOT_EQUAL_MEM
 * ASSERT_TRUE
 * ASSERT_FALSE
 */

/** You HAVE to pass argc and argv to init. It is needed to parse `--filter` and
 *  more.
 */
int main(int argc, char **argv) {
  exam_init(argc, argv);
  return exam_run();
}
```

Build and like your source code against `examiner`

```bash
cc test_file.c -o test -lexaminer
```

If you get
```
./test: error while loading shared libraries: libexaminer.so: cannot open shared object file: No such file or directory
```
then your `LD_LIBRARY_PATH` is probably missing `/usr/local/bin`
```bash
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib ./test

# To see help page run
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib ./test --help

# To filter for a test
# The first will run all tests under the math scope.
# The second will run all tests that start with math.test_int (probably just one)
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib ./test --filter math
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/local/lib ./test --filter math.test_int
```

## TODO:

- [x] More test definitions `PENDING`
- [ ] `BEFORE_EACH` and `AFTER_EACH`
- [x] Assert more than just int
- [x] `ASSERT_TRUE` and `ASSERT_FALSE`
- [ ] `ASSERT_LIST` with `...` / `__VA_ARGS__`
- [x] When filtering it will still say the full filter count at the beginning
- [x] Auto register TEST, no idea how. Good Luck to myself
- [ ] Easy multifile support
- [x] `--filter` needs substr matching so i can only run one namespace
- [x] timer how loong the execution of a test took
- [ ] separator between namespaces (either a `\n` or `string.rep(78, '-')`
  - [ ] better grouping even if they are not sorted in the file.
        (scope grouping)
- [x] Documentation
  - [x] Guide in github
  - [x] `--help`
- [x] enable/disable color. on / off
- [x] `--list-tests`
- [ ] `--short`
- [ ] `--shuffle` test order
- [x] `--repeat` run tests multiple times
- [x] `--die-on-failure` stop executing on fail
- [x] make install/uninstall
