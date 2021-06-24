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
    printf("\e[32m[       OK ] %s\e[0m\n", __internal_test_name);              \
    return true;                                                               \
  }

exam_env _exam_init(int argc, char **argv, void *tests[], size_t count);
int _exam_run(const exam_env *env);

#define exam_init(argc, argv, array)                                           \
  _exam_init(argc, argv, array, sizeof(array) / sizeof(array[0]))
#define exam_run(env) _exam_run(env)

void _exam_assert_equal_double(double expected, double result,
                               const char *test_name, const char *file,
                               int line);
void _exam_assert_equal_float(float expected, float result,
                              const char *test_name, const char *file,
                              int line);
void _exam_assert_equal_int(int expected, int result, const char *test_name,
                            const char *file, int line);
void _exam_assert_equal_str(const char *expected, const char *result,
                            const char *test_name, const char *file, int line);
void _exam_assert_equal_mem(void *expected, void *result, size_t len,
                            const char *test_name, const char *file, int line);

#define __ISTYPE(x, t) __builtin_types_compatible_p(typeof(x), t)

// clang-format off
#define ASSERT_EQUAL(expected, result)                                         \
  _Static_assert(                                                              \
    __ISTYPE(expected, typeof(result)),                                        \
    "'expected' and 'result' need to be from the same type!"                   \
  );                                                                           \
  (                                                                            \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, double), _exam_assert_equal_double,                   \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, float), _exam_assert_equal_float,                     \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, int), _exam_assert_equal_int,                         \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, char[]), _exam_assert_equal_str,                      \
    (void)0))))                                                                \
  (expected, result, __internal_test_name, __FILE__, __LINE__))
// clang-format on
#define ASSERT_EQUAL_MEM(expected, result, len)                                \
  _exam_assert_equal_mem(expected, result, len, __internal_test_name,          \
                         __FILE__, __LINE__)

void _exam_assert_not_equal_double(double expected, double result,
                                   const char *test_name, const char *file,
                                   int line);
void _exam_assert_not_equal_float(float expected, float result,
                                  const char *test_name, const char *file,
                                  int line);
void _exam_assert_not_equal_int(int expected, int result, const char *test_name,
                                const char *file, int line);
void _exam_assert_not_equal_str(const char *expected, const char *result,
                                const char *test_name, const char *file,
                                int line);
void _exam_assert_not_equal_mem(void *expected, void *result, size_t len,
                                const char *test_name, const char *file,
                                int line);

#define __ISTYPE(x, t) __builtin_types_compatible_p(typeof(x), t)

// clang-format off
#define ASSERT_NOT_EQUAL(expected, result)                                     \
  _Static_assert(                                                              \
    __ISTYPE(expected, typeof(result)),                                        \
    "'expected' and 'result' need to be from the same type!"                   \
  );                                                                           \
  (                                                                            \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, double), _exam_assert_not_equal_double,               \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, float), _exam_assert_not_equal_float,                 \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, int), _exam_assert_not_equal_int,                     \
    __builtin_choose_expr(                                                     \
      __ISTYPE(expected, char[]), _exam_assert_not_equal_str,                  \
    (void)0))))                                                                \
  (expected, result, __internal_test_name, __FILE__, __LINE__))
// clang-format on
#define ASSERT_NOT_EQUAL_MEM(expected, result, len)                            \
  _exam_assert_not_equal_mem(expected, result, len, __internal_test_name,      \
                             __FILE__, __LINE__)

// TODO(conni2461): NOT equal

#endif // EXAMINER_H
