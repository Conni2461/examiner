#include "examiner.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const static double EPSILON = 0.0001;

static sigjmp_buf global_env;
static exam_test_list_t global_tests = {0};

static char *exam_concat_scope_name(const exam_test_t *test) {
  size_t buf_len = strlen(test->scope) + strlen(test->name) + 2;
  char *buf = malloc(sizeof(char) * buf_len);
  memset(buf, 0, buf_len);
  strcat(buf, test->scope);
  strcat(buf, ".");
  strcat(buf, test->name);

  return buf;
}

static bool exam_filter_test(const char *name, const char *filter) {
  int i = 0;
  while (true) {
    if (filter[i] == '\0') {
      break;
    }
    if (name[i] != filter[i]) {
      return false;
    }
    i++;
  }
  return true;
}

exam_env exam_init(int argc, char **argv) {
  const char *filter = NULL;
  if (argc == 3) {
    if (strncmp(argv[1], "--filter", 8) == 0) {
      filter = argv[2];
    }
  }
  return (exam_env){.filter = filter};
}

int exam_run(const exam_env *env) {
  int retValue = 0;
  size_t count = 0, passed = 0;
  if (env->filter != NULL) {
    for (size_t i = 0; i < global_tests.len; ++i) {
      char *buf = exam_concat_scope_name(&global_tests.tests[i]);
      if (exam_filter_test(buf, env->filter)) {
        ++count;
      }
      free(buf);
    }
  } else {
    count = global_tests.len;
  }

  printf("\e[90m[==========] Running %zu test(s)\e[0m\n", count);
  for (size_t i = 0; i < global_tests.len; ++i) {
    char *buf = exam_concat_scope_name(&global_tests.tests[i]);
    if (env->filter != NULL && !exam_filter_test(buf, env->filter)) {
      free(buf);
      continue;
    }
    if (global_tests.tests[i].pending) {
      printf("\e[34m[ PENDING  ] \e[1m%s\e[0m\n", buf);
      free(buf);
      continue;
    }
    if (sigsetjmp(global_env, 1) == 0) {
      printf("\e[90m[ RUN      ] \e[1m%s\e[0m\n", buf);
      global_tests.tests[i].fn(buf);
      ++passed;
      printf("\e[32m[       OK ] %s\e[0m\n", buf);
    } else {
      retValue = 1;
    }
    free(buf);
  }

  printf("\e[32m[  PASSED  ] %zu test(s)\e[0m\n", passed);
  printf("\e[90m[==========] Ran %zu test(s)\e[0m\n", count);

  return retValue;
}

void _exam_register_test(const char *scope, const char *name, exam_test_fn fn,
                         bool pending) {
  if (global_tests.tests == NULL) {
    global_tests.tests = malloc(sizeof(exam_test_list_t) * 16);
    global_tests.len = 0;
    global_tests.cap = 16;
  } else {
    if (global_tests.len + 1 >= global_tests.cap) {
      global_tests.cap *= 2;
      global_tests.tests = realloc(global_tests.tests, global_tests.cap);
    }
  }

  global_tests.tests[global_tests.len++] =
      (exam_test_t){.fn = fn, .scope = scope, .name = name, .pending = pending};
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
