#include "examiner.h"

#include "math.h"
#include "string.h"

const static double EPSILON = 0.0001;

static sigjmp_buf global_env;

exam_env _exam_init(int argc, char **argv, void *tests[], size_t count) {
  const char *filter = NULL;
  if (argc == 3) {
    if (strncmp(argv[1], "--filter", 8) == 0) {
      filter = argv[2];
    }
  }
  return (exam_env){.filter = filter, .tests = tests, .test_count = count};
}

int _exam_run(const exam_env *env) {
  int retValue = 0;
  size_t final_count = env->test_count, passed = 0;

  printf("\e[90m[==========] Running %zu test(s)\e[0m\n", env->test_count);
  for (size_t i = 0; i < env->test_count; ++i) {
    if (sigsetjmp(global_env, 1) == 0) {
      bool ran = ((bool (*)(const char *))env->tests[i])(env->filter);
      if (!ran) {
        --final_count;
      } else {
        ++passed;
      }
    } else {
      retValue = 1;
    }
  }

  printf("\e[32m[  PASSED  ] %zu test(s)\e[0m\n", passed);
  printf("\e[90m[==========] Ran %zu test(s)\e[0m\n", final_count);

  return retValue;
}

void _exam_assert_equal_double(double expected, double result,
                               const char *test_name, const char *file,
                               int line) {
  if (fabs(expected - result) >= EPSILON) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %f \e[31mResult: %f\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_float(float expected, float result,
                              const char *test_name, const char *file,
                              int line) {
  if (fabs(expected - result) >= EPSILON) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %f \e[31mResult: %f\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_int(int expected, int result, const char *test_name,
                            const char *file, int line) {
  if (expected != result) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %d \e[31mResult: %d\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_str(const char *expected, const char *result,
                            const char *test_name, const char *file, int line) {
  if (strcmp(expected, result) != 0) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %s \e[31mResult: %s\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_mem(void *expected, void *result, size_t len,
                            const char *test_name, const char *file, int line) {
  const char *a = (const char *)expected;
  const char *b = (const char *)result;

  size_t differences = 0;
  for (size_t i = 0; i < len; i++) {
    const char l = a[i];
    const char r = b[i];
    if (l != r) {
      if (differences < 16) {
        if (differences == 0) {
          printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
          printf("Error at line: %s:%d\n", file, line);
        }
        printf("difference at offset %zd 0x%02x != 0x%02x\n", i, l, r);
      }
      differences++;
    }
  }
  if (differences > 0) {
    if (differences >= 16) {
      printf("...\n");
    }
    printf("%zd bytes of %p and %p are different\n", differences, (void *)a,
           (void *)b);

    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_double(double expected, double result,
                                   const char *test_name, const char *file,
                                   int line) {
  if (fabs(expected - result) < EPSILON) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %f \e[31mResult: %f\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_float(float expected, float result,
                                  const char *test_name, const char *file,
                                  int line) {
  if (fabs(expected - result) < EPSILON) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %f \e[31mResult: %f\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_int(int expected, int result, const char *test_name,
                                const char *file, int line) {
  if (expected == result) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %d \e[31mResult: %d\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_str(const char *expected, const char *result,
                                const char *test_name, const char *file,
                                int line) {
  if (strcmp(expected, result) == 0) {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %s \e[31mResult: %s\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_mem(void *expected, void *result, size_t len,
                                const char *test_name, const char *file,
                                int line) {
  const char *a = (const char *)expected;
  const char *b = (const char *)result;

  size_t differences = 0;
  for (size_t i = 0; i < len; i++) {
    const char l = a[i];
    const char r = b[i];
    if (l == r) {
      if (differences < 16) {
        if (differences == 0) {
          printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
          printf("Error at line: %s:%d\n", file, line);
        }
        printf("same at offset %zd 0x%02x != 0x%02x\n", i, l, r);
      }
      differences++;
    }
  }
  if (differences > 0) {
    if (differences >= 16) {
      printf("...\n");
    }
    printf("%zd bytes of %p and %p are same\n", differences, (void *)a,
           (void *)b);

    siglongjmp(global_env, 1);
  }
}
