#ifndef EXAMINER_H
#define EXAMINER_H

#include <setjmp.h>
#include <stddef.h>
#include <stdbool.h>

typedef void (*exam_test_fn)();

typedef struct {
  const char *filter;
} exam_env;

typedef struct {
  exam_test_fn fn;
  const char *scope;
  const char *name;
  bool pending;
} exam_test_t;

typedef struct {
  exam_test_t *tests;
  size_t len;
  size_t cap;
} exam_test_list_t;

exam_env exam_init(int argc, char **argv);
int exam_run(const exam_env *env);
void _exam_register_test(const char *scope, const char *name, exam_test_fn fn,
                         bool pending);

#define __REGISTER_TEST(SCOPE, NAME, PENDING)                                  \
  void examtest_##SCOPE##_##NAME();                                            \
  void examtest_##SCOPE##_##NAME##_register() __attribute__((constructor));    \
  void examtest_##SCOPE##_##NAME##_register() {                                \
    _exam_register_test(#SCOPE, #NAME, &examtest_##SCOPE##_##NAME, PENDING);   \
  }                                                                            \
  void examtest_##SCOPE##_##NAME()

#define TEST(SCOPE, NAME) __REGISTER_TEST(SCOPE, NAME, false)
#define PENDING(SCOPE, NAME) __REGISTER_TEST(SCOPE, NAME, true)

/////////////
// ASSERTS //
/////////////
void _exam_assert_equal_double(double expected, double result, const char *file,
                               int line);
void _exam_assert_equal_float(float expected, float result, const char *file,
                              int line);
void _exam_assert_equal_int(int expected, int result, const char *file,
                            int line);
void _exam_assert_equal_str(const char *expected, const char *result,
                            const char *file, int line);
void _exam_assert_equal_mem(void *expected, void *result, size_t len,
                            const char *file, int line);

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
  (expected, result, __FILE__, __LINE__))
// clang-format on
#define ASSERT_EQUAL_MEM(expected, result, len)                                \
  _exam_assert_equal_mem(expected, result, len, __FILE__, __LINE__)

void _exam_assert_not_equal_double(double expected, double result,
                                   const char *file, int line);
void _exam_assert_not_equal_float(float expected, float result,
                                  const char *file, int line);
void _exam_assert_not_equal_int(int expected, int result, const char *file,
                                int line);
void _exam_assert_not_equal_str(const char *expected, const char *result,
                                const char *file, int line);
void _exam_assert_not_equal_mem(void *expected, void *result, size_t len,
                                const char *file, int line);

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
  (expected, result, __FILE__, __LINE__))
// clang-format on
#define ASSERT_NOT_EQUAL_MEM(expected, result, len)                            \
  _exam_assert_not_equal_mem(expected, result, len, __FILE__, __LINE__)

#endif // EXAMINER_H
