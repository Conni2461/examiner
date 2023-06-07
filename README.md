# examiner

A small, opinionated c unit testing framework.

## Features

- Autoregister tests
- `ASSERT_EQ` and `ASSERT_NE` macros that dispatches a compare to the
appropriated type
- `BEFORE_EACH` and `AFTER_EACH` to register functions that run before and/or
after a scope
- `PENDING` to tell examiner that a test should not run
- colored output support
- time how long it took to run a test
- multiple files just work. Define one `main.c` file and n test files and build
one executable. It should execute all tests without additional setup or header
files. See makefile and `test` directory for an example
- parsing argc argv for parameter options
  - filter for test with `--filter pattern`, possible to only filter for a scope
  thanks to prefix matching. Multiple `--filter` will act like an or, so only
  one filter needs to match in order to get a test to execute.
  - list all tests with `--list-tests`
  - short output with `--short`
  - shuffle test order with `--shuffle`
  - repeat test with `--repeat n`
  - die on fail with `--die-on-fail`
  - disable colors with `--color off`

![preview](https://i.imgur.com/9vcLkOp.png)

## Build

```bash
make                # set optimazation with TYPE=, example: TYPE=-Og. Default is: `-O2`
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
  /** ASSERT_EQ and ASSERT_NE are two macros that will dispatch based on the
   *  type to internal comparator functions. It dispatches
   *  `int32_t`, `uint32_t`,
   *  `long int`, `unsigned long int`,
   *  `long long`, `unsigned long long`,
   *  `float`, `double`,
   *  `char`, `char *`
   */
  ASSERT_EQ(1, 1);
  ASSERT_NE(1, 2);
}

/**
 * You can define BEFORE_EACH and AFTER_EACH for each scope (even after a test
 * definition). Before each and after each will run before and after each test
 * in the "math" scope (for this example)
 */
BEFORE_EACH(math) {
  printf("Before math POG\n");
}

AFTER_EACH(math) {
  printf("After math POG\n");
}

TEST(str, test) {
  ASSERT_EQ("test", "test");
  ASSERT_NE("test", "nest");
}

/**
 * PENDING means that the feature the test is testing is probably not
 * implemented yet so the test will not be executed.
 */
PENDING(pending, should_fail) {
  ASSERT_EQ("a", "b");
}

/**
 * Also awailable are:
 * ASSERT_EQ_MEM
 * ASSERT_NE_MEM
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

This can also be used in cmake projects: either clone this repository and do

```cmake
add_subdirectory(examiner)
target_link_libraries(your_target PRIVATE examiner)
```

or install cmake config files

```bash
cmake -B build
cmake --build build
sudo cmake --install build --prefix /usr/local
```

and use `find_package`:

```cmake
find_package(examiner REQUIRED)
target_link_libraries(your_target PRIVATE examiner)
```
