#include "examiner.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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

#define NONE env->color(0)
#define RED env->color(31)
#define GREEN env->color(32)
#define BLUE env->color(34)
#define GRAY env->color(90)

static const char *colored_matcher(int value) {
  switch (value) {
  case 0: return "\e[0m";
  case 31: return "\e[31m";
  case 32: return "\e[32m";
  case 34: return "\e[34m";
  case 90: return "\e[90m";
  }
  return "\e[0m";
}

static const char *non_colored_matcher(int value) {
  return "";
}

exam_env_t exam_init(int argc, char **argv) {
  const char *filter = NULL;
  exam_color_matcher color = &colored_matcher;
  int32_t repeat = 1;
  bool list = false;
  bool shuffel = false;
  bool shortd = false;
  bool die_on_fail = false;
  if (argc > 1) {
    for (size_t i = 1; i < argc; ++i) {
      if (strncmp(argv[i], "--list-tests", 12) == 0) {
        list = true;
      } else if (strncmp(argv[i], "--short", 7) == 0) {
        shortd = true;
      } else if (strncmp(argv[i], "--filter", 8) == 0) {
        if ((i + 1) == argc) {
          fprintf(stderr, "--filter requires a second parameter\n");
          exit(1);
        }
        // TODO(conni2461): Allow to filter more than onces
        filter = argv[++i];
      } else if (strncmp(argv[i], "--shuffel", 9) == 0) {
        shuffel = true;
      } else if (strncmp(argv[i], "--repeat", 8) == 0) {
        if ((i + 1) == argc) {
          fprintf(stderr, "--repeat requires a second parameter\n");
          exit(1);
        }
        repeat = atoi(argv[++i]);
        if (repeat == 0) {
          fprintf(stderr, "repeat is not a number or 0 is not a valid input. "
                          "Input has to be >= 1\n");
          exit(1);
        }
      } else if (strncmp(argv[i], "--die-on-fail", 13) == 0) {
        die_on_fail = true;
      } else if (strncmp(argv[i], "--color", 7) == 0) {
        if ((i + 1) == argc) {
          fprintf(stderr, "--color requires a second parameter! on|off\n");
          exit(1);
        }
        ++i;
        if (strncmp(argv[i], "on", 2) == 0) {
          color = &colored_matcher;
        } else if (strncmp(argv[i], "off", 3) == 0) {
          color = &non_colored_matcher;
        } else {
          fprintf(
              stderr,
              "second parameter passed to color is neither `on` or `off`\n");
          exit(1);
        }
      } else if (strncmp(argv[i], "--help", 6) == 0 ||
                 strncmp(argv[i], "-h", 2) == 0) {
        printf("%s [options]\n"
               "  --list-tests      only list all tests\n"
               "  --short           short output\n"
               "  --filter [str]    filter for one or many tests (substr "
               "matching)\n"
               "  --shuffel         shuffel test execution order\n"
               "  --repeat [n]      repeat all tests n times\n"
               "  --die-on-fail     stop execution on failure\n"
               "\n"
               "  --color [on, off] color output. Default: on\n"
               "\n"
               "  -h | --help       print help page\n"
               "  -v | --version    print software version\n",
               argv[0]);
        exit(0);
      } else if (strncmp(argv[i], "--version", 8) == 0 ||
                 strncmp(argv[i], "-v", 2) == 0) {
        printf("Version 0.1 License MIT (conni2461)\n");
        exit(0);
      } else {
        fprintf(stderr, "Option %s not found! See -h for supported options",
                argv[i]);
        exit(1);
      }
    }
  }
  return (exam_env_t){.filter = filter,
                      .color = color,
                      .repeat = repeat,
                      .list = list,
                      .shuffel = shuffel,
                      .shortd = shortd,
                      .die_on_fail = die_on_fail};
}

int exam_run(const exam_env_t *env) {
  int retValue = 0;
  size_t count = 0, passed = 0;
  if (env->filter != NULL || env->list) {
    for (size_t i = 0; i < global_tests.len; ++i) {
      char *buf = exam_concat_scope_name(&global_tests.tests[i]);
      if (env->filter != NULL) {
        if (exam_filter_test(buf, env->filter)) {
          if (env->list) {
            printf("%s\n", buf);
          }
          ++count;
        }
      } else {
        printf("%s\n", buf);
      }
      free(buf);
    }
  } else {
    count = global_tests.len;
  }

  if (env->list) {
    exit(0);
  }

  printf("%s[==========] Running %zu test(s)%s\n", GRAY, count, NONE);
  for (size_t i = 0; i < global_tests.len; ++i) {
    char *buf = exam_concat_scope_name(&global_tests.tests[i]);
    if (env->filter != NULL && !exam_filter_test(buf, env->filter)) {
      free(buf);
      continue;
    }
    if (global_tests.tests[i].pending) {
      printf("%s[ PENDING  ] %s%s\n", BLUE, NONE, buf);
      free(buf);
      continue;
    }
    if (sigsetjmp(global_env, 1) == 0) {
      printf("%s[ RUN      ] %s%s\n", GRAY, NONE, buf);
      double diff;
      for (size_t i = 0; i < env->repeat; i++) {
        clock_t start = clock();
        global_tests.tests[i].fn(env);
        clock_t end = clock();
        diff = ((double)(end - start)) / CLOCKS_PER_SEC;
      }
      ++passed;
      printf("%s[       OK ] %s%s [%.2f s]\n", GREEN, NONE, buf, diff);
    } else {
      printf("%s[  FAILED  ] %s%s\n", RED, NONE, buf);
      retValue = 1;
    }
    free(buf);
  }

  printf("%s[  PASSED  ] %zu test(s)%s\n", GREEN, passed, NONE);
  printf("%s[==========] Ran %zu test(s)%s\n", GRAY, count, NONE);

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

void _exam_assert_equal_double(double expected, double result, const char *file,
                               int line, const exam_env_t *env) {
  if (fabs(expected - result) >= EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_float(float expected, float result, const char *file,
                              int line, const exam_env_t *env) {
  if (fabs(expected - result) >= EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_int(int expected, int result, const char *file,
                            int line, const exam_env_t *env) {
  if (expected != result) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %d %sResult: %d%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_str(const char *expected, const char *result,
                            const char *file, int line, const exam_env_t *env) {
  if (strcmp(expected, result) != 0) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %s %sResult: %s%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_equal_mem(void *expected, void *result, size_t len,
                            const char *file, int line, const exam_env_t *env) {
  const char *a = (const char *)expected;
  const char *b = (const char *)result;

  size_t differences = 0;
  for (size_t i = 0; i < len; i++) {
    const char l = a[i];
    const char r = b[i];
    if (l != r) {
      if (differences < 16) {
        if (differences == 0) {
          printf("  Error at line: %s:%d\n", file, line);
        }
        printf("  difference at offset %zd 0x%02x != 0x%02x\n", i, l, r);
      }
      differences++;
    }
  }
  if (differences > 0) {
    if (differences >= 16) {
      printf("  ...\n");
    }
    printf("  %zd bytes of %p and %p are different\n", differences, (void *)a,
           (void *)b);

    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_double(double expected, double result,
                                   const char *file, int line,
                                   const exam_env_t *env) {
  if (fabs(expected - result) < EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_float(float expected, float result,
                                  const char *file, int line,
                                  const exam_env_t *env) {
  if (fabs(expected - result) < EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_int(int expected, int result, const char *file,
                                int line, const exam_env_t *env) {
  if (expected == result) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %d %sResult: %d%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_str(const char *expected, const char *result,
                                const char *file, int line,
                                const exam_env_t *env) {
  if (strcmp(expected, result) == 0) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %s %sResult: %s%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_env, 1);
  }
}

void _exam_assert_not_equal_mem(void *expected, void *result, size_t len,
                                const char *file, int line,
                                const exam_env_t *env) {
  const char *a = (const char *)expected;
  const char *b = (const char *)result;

  size_t differences = 0;
  for (size_t i = 0; i < len; i++) {
    const char l = a[i];
    const char r = b[i];
    if (l == r) {
      if (differences < 16) {
        if (differences == 0) {
          printf("  Error at line: %s:%d\n", file, line);
        }
        printf("  same at offset %zd 0x%02x != 0x%02x\n", i, l, r);
      }
      differences++;
    }
  }
  if (differences > 0) {
    if (differences >= 16) {
      printf("  ...\n");
    }
    printf("  %zd bytes of %p and %p are same\n", differences, (void *)a,
           (void *)b);

    siglongjmp(global_env, 1);
  }
}
