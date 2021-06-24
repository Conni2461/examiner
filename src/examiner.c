#include "examiner.h"

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

void _exam_assert_equal(int expected, int result, const char *test_name,
                        const char *file, int line) {
  if (expected == result) {
    printf("\e[32m[    OK    ] %s\e[0m\n", test_name);
  } else {
    printf("\e[31m[  FAILED  ] %s\e[0m\n", test_name);
    printf("Error at line: %s:%d\n", file, line);
    printf("\e[32mExpected: %d \e[31mResult: %d\e[0m\n", expected, result);
    siglongjmp(global_env, 1);
  }
}
