#include "examiner.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

const static double EPSILON = 0.0001;

static sigjmp_buf global_sig;
static exam_env_t global_env = {0};

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

#define NONE global_env.color(0)
#define RED global_env.color(31)
#define GREEN global_env.color(32)
#define BLUE global_env.color(34)
#define GRAY global_env.color(90)

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

void exam_init(int argc, char **argv) {
  global_env.filter = NULL;
  global_env.color = &colored_matcher;
  global_env.repeat = 1;
  global_env.list = false;
  global_env.shuffel = false;
  global_env.shortd = false;
  global_env.die_on_fail = false;
  if (argc > 1) {
    for (size_t i = 1; i < argc; ++i) {
      if (strncmp(argv[i], "--list-tests", 12) == 0) {
        global_env.list = true;
      } else if (strncmp(argv[i], "--short", 7) == 0) {
        global_env.shortd = true;
      } else if (strncmp(argv[i], "--filter", 8) == 0) {
        if ((i + 1) == argc) {
          fprintf(stderr, "--filter requires a second parameter\n");
          exit(1);
        }
        // TODO(conni2461): Allow to filter more than onces
        global_env.filter = argv[++i];
      } else if (strncmp(argv[i], "--shuffel", 9) == 0) {
        global_env.shuffel = true;
      } else if (strncmp(argv[i], "--repeat", 8) == 0) {
        if ((i + 1) == argc) {
          fprintf(stderr, "--repeat requires a second parameter\n");
          exit(1);
        }
        global_env.repeat = atoi(argv[++i]);
        if (global_env.repeat == 0) {
          fprintf(stderr, "repeat is not a number or 0 is not a valid input. "
                          "Input has to be >= 1\n");
          exit(1);
        }
      } else if (strncmp(argv[i], "--die-on-fail", 13) == 0) {
        global_env.die_on_fail = true;
      } else if (strncmp(argv[i], "--color", 7) == 0) {
        if ((i + 1) == argc) {
          fprintf(stderr, "--color requires a second parameter! on|off\n");
          exit(1);
        }
        ++i;
        if (strncmp(argv[i], "on", 2) == 0) {
          global_env.color = &colored_matcher;
        } else if (strncmp(argv[i], "off", 3) == 0) {
          global_env.color = &non_colored_matcher;
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
}

int exam_run() {
  int retValue = 0;
  size_t count = 0, passed = 0;
  if (global_env.filter != NULL || global_env.list) {
    for (size_t i = 0; i < global_env.tbl.len; ++i) {
      for (size_t j = 0; j < global_env.tbl.scope[i].len; ++j) {
        char *buf = exam_concat_scope_name(&global_env.tbl.scope[i].tests[j]);
        if (global_env.filter != NULL) {
          if (exam_filter_test(buf, global_env.filter)) {
            if (global_env.list) {
              printf("%s\n", buf);
            }
            ++count;
          }
        } else {
          printf("%s\n", buf);
        }
        free(buf);
      }
    }
  } else {
    for (size_t i = 0; i < global_env.tbl.len; ++i) {
      count += global_env.tbl.scope[i].len;
    }
  }

  if (global_env.list) {
    exit(0);
  }

  printf("%s[==========] Running %zu test(s)%s\n", GRAY, count, NONE);
  for (size_t i = 0; i < global_env.tbl.len; ++i) {
    printf("%s[==========] Running %zu test(s) in scope %s%s\n", GRAY,
           global_env.tbl.scope[i].len, global_env.tbl.scope[i].name, NONE);
    size_t scope_passed = 0;
    for (size_t j = 0; j < global_env.tbl.scope[i].len; ++j) {
      exam_test_t *current_test = &global_env.tbl.scope[i].tests[j];
      char *buf = exam_concat_scope_name(current_test);
      if (global_env.filter != NULL &&
          !exam_filter_test(buf, global_env.filter)) {
        free(buf);
        continue;
      }
      if (current_test->pending) {
        printf("%s[ PENDING  ] %s%s\n", BLUE, NONE, buf);
        free(buf);
        continue;
      }
      if (sigsetjmp(global_sig, 1) == 0) {
        printf("%s[ RUN      ] %s%s\n", GRAY, NONE, buf);
        double diff;
        for (size_t k = 0; k < global_env.repeat; k++) {
          clock_t start = clock();
          current_test->fn();
          clock_t end = clock();
          diff = ((double)(end - start)) / CLOCKS_PER_SEC;
        }
        ++passed;
        ++scope_passed;
        printf("%s[       OK ] %s%s [%.2f s]\n", GREEN, NONE, buf, diff);
      } else {
        printf("%s[  FAILED  ] %s%s\n", RED, NONE, buf);
        retValue = 1;
        if (global_env.die_on_fail) {
          exit(1);
        }
      }
      free(buf);
    }
    printf("%s[  PASSED  ] %zu test(s) passed in scope %s%s\n", GREEN,
           scope_passed, global_env.tbl.scope[i].name, NONE);
    printf("\n");
  }

  printf("%s[  PASSED  ] %zu test(s) across all scopes%s\n", GREEN, passed,
         NONE);
  printf("%s[==========] Ran %zu test(s) across all scopes%s\n", GRAY, count,
         NONE);

  return retValue;
}

static void exam_insert_test_in_list(exam_scope_t *list, const char *scope,
                                     const char *name, void (*fn)(),
                                     bool pending) {
  if (list->tests == NULL) {
    list->cap = 16;
    list->tests = malloc(sizeof(exam_scope_t) * list->cap);
    list->len = 0;
  } else {
    if (list->len + 1 >= list->cap) {
      list->cap *= 2;
      list->tests = realloc(list->tests, sizeof(exam_scope_t) * list->cap);
    }
  }

  list->tests[list->len++] =
      (exam_test_t){.fn = fn, .scope = scope, .name = name, .pending = pending};
}

void _exam_register_test(const char *scope, const char *name, void (*fn)(),
                         bool pending) {
  if (global_env.tbl.scope == NULL) {
    global_env.tbl.cap = 8;
    global_env.tbl.scope = malloc(sizeof(exam_scope_t) * global_env.tbl.cap);
    memset(global_env.tbl.scope, 0, sizeof(exam_scope_t) * global_env.tbl.cap);
    global_env.tbl.len = 0;
  }

  size_t idx = 0;
  for (; idx < global_env.tbl.len; ++idx) {
    if (strcmp(scope, global_env.tbl.scope[idx].name) == 0) {
      exam_insert_test_in_list(&global_env.tbl.scope[idx], scope, name, fn,
                               pending);
      return;
    }
  }
  // At this point we have not found it in our simple table
  if (global_env.tbl.len + 1 >= global_env.tbl.cap) {
    global_env.tbl.cap *= 2;
    global_env.tbl.scope = realloc(global_env.tbl.scope,
                                   sizeof(exam_scope_t) * global_env.tbl.cap);
  }

  global_env.tbl.scope[global_env.tbl.len] =
      (exam_scope_t){.tests = NULL, .len = 0, .cap = 0, .name = scope};
  exam_insert_test_in_list(&global_env.tbl.scope[global_env.tbl.len++], scope,
                           name, fn, pending);
}

void _exam_assert_true(bool value, const char *file, int line) {
  if (!value) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sexpected: true %sreceived: false %s\n", GREEN, RED, NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_false(bool value, const char *file, int line) {
  if (value) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sexpected: false %sreceived: true %s\n", GREEN, RED, NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_equal_double(double expected, double result, const char *file,
                               int line) {
  if (fabs(expected - result) >= EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_equal_float(float expected, float result, const char *file,
                              int line) {
  if (fabs(expected - result) >= EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_equal_int(int expected, int result, const char *file,
                            int line) {
  if (expected != result) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %d %sResult: %d%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_equal_str(const char *expected, const char *result,
                            const char *file, int line) {
  if (strcmp(expected, result) != 0) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %s %sResult: %s%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_equal_mem(void *expected, void *result, size_t len,
                            const char *file, int line) {
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

    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_not_equal_double(double expected, double result,
                                   const char *file, int line) {
  if (fabs(expected - result) < EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_not_equal_float(float expected, float result,
                                  const char *file, int line) {
  if (fabs(expected - result) < EPSILON) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %f %sResult: %f%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_not_equal_int(int expected, int result, const char *file,
                                int line) {
  if (expected == result) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %d %sResult: %d%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_not_equal_str(const char *expected, const char *result,
                                const char *file, int line) {
  if (strcmp(expected, result) == 0) {
    printf("  Error at line: %s:%d\n", file, line);
    printf("  %sExpected: %s %sResult: %s%s\n", GREEN, expected, RED, result,
           NONE);
    siglongjmp(global_sig, 1);
  }
}

void _exam_assert_not_equal_mem(void *expected, void *result, size_t len,
                                const char *file, int line) {
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

    siglongjmp(global_sig, 1);
  }
}
