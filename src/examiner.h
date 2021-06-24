#ifndef EXAMINER_H
#define EXAMINER_H

#include <setjmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const char *filter;
  void **tests;
  size_t test_count;
} exam_env;

#define DUMP_BOOL(x) x ? "true" : "false"

#define TEST(namespace, test_name, y)                                          \
  bool namespace##_##test_name(const char *filter) {                           \
    const char *__internal_test_name = #namespace "." #test_name;              \
    if (filter) {                                                              \
      if (strcmp(filter, __internal_test_name) != 0) {                         \
        return false;                                                          \
      }                                                                        \
    }                                                                          \
                                                                               \
    printf("\e[90m[ RUN      ] \e[1m%s\e[0m\n", __internal_test_name);         \
    y;                                                                         \
    return true;                                                               \
  }

exam_env _exam_init(int argc, char **argv, void *tests[], size_t count);
int _exam_run(const exam_env *env);

void _exam_assert_equal(int expected, int result, const char *test_name,
                        const char *file, int line);

#define exam_init(argc, argv, array)                                           \
  _exam_init(argc, argv, array, sizeof(array) / sizeof(array[0]))
#define exam_run(env) _exam_run(env)

#define ASSERT_EQUAL(expected, result)                                         \
  _exam_assert_equal(expected, result, __internal_test_name, __FILE__, __LINE__)

#endif // EXAMINER_H
