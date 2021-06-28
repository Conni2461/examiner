#ifndef EXAMINER_H
#define EXAMINER_H

#include <setjmp.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  const char *filter;
  char *(*color)(int);
  int32_t repeat;
  bool list;
  bool shuffel;     // TODO(conni2461):
  bool shortd;      // TODO(conni2461):
  bool die_on_fail; // TODO(conni2461):
} exam_env_t;

typedef struct {
  void (*fn)(const exam_env_t *);
  const char *scope;
  const char *name;
  bool pending;
} exam_test_t;

typedef struct {
  exam_test_t *tests;
  size_t len;
  size_t cap;
} exam_test_list_t;

exam_env_t exam_init(int argc, char **argv);
int exam_run(const exam_env_t *env);
void _exam_register_test(const char *scope, const char *name,
                         void (*fn)(const exam_env_t *), bool pending);

#define __REGISTER_TEST(SCOPE, NAME, PENDING)                                  \
  void examtest_##SCOPE##_##NAME(const exam_env_t *);                          \
  void examtest_##SCOPE##_##NAME##_register() __attribute__((constructor));    \
  void examtest_##SCOPE##_##NAME##_register() {                                \
    _exam_register_test(#SCOPE, #NAME, &examtest_##SCOPE##_##NAME, PENDING);   \
  }                                                                            \
  void examtest_##SCOPE##_##NAME(const exam_env_t *__exam_env)

#define TEST(SCOPE, NAME) __REGISTER_TEST(SCOPE, NAME, false)
#define PENDING(SCOPE, NAME) __REGISTER_TEST(SCOPE, NAME, true)

/////////////
// ASSERTS //
/////////////
void _exam_assert_true(bool value, const char *file, int line,
                       const exam_env_t *env);
void _exam_assert_false(bool value, const char *file, int line,
                        const exam_env_t *env);

#define ASSERT_TRUE(x) _exam_assert_true(x, __FILE__, __LINE__, __exam_env);
#define ASSERT_FALSE(x) _exam_assert_false(x, __FILE__, __LINE__, __exam_env);

void _exam_assert_equal_double(double expected, double result, const char *file,
                               int line, const exam_env_t *env);
void _exam_assert_equal_float(float expected, float result, const char *file,
                              int line, const exam_env_t *env);
void _exam_assert_equal_int(int expected, int result, const char *file,
                            int line, const exam_env_t *env);
void _exam_assert_equal_str(const char *expected, const char *result,
                            const char *file, int line, const exam_env_t *env);
void _exam_assert_equal_mem(void *expected, void *result, size_t len,
                            const char *file, int line, const exam_env_t *env);

void _exam_assert_not_equal_double(double expected, double result,
                                   const char *file, int line,
                                   const exam_env_t *env);
void _exam_assert_not_equal_float(float expected, float result,
                                  const char *file, int line,
                                  const exam_env_t *env);
void _exam_assert_not_equal_int(int expected, int result, const char *file,
                                int line, const exam_env_t *env);
void _exam_assert_not_equal_str(const char *expected, const char *result,
                                const char *file, int line,
                                const exam_env_t *env);
void _exam_assert_not_equal_mem(void *expected, void *result, size_t len,
                                const char *file, int line,
                                const exam_env_t *env);

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
  (expected, result, __FILE__, __LINE__, __exam_env))

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
  (expected, result, __FILE__, __LINE__, __exam_env))
// clang-format on

#define ASSERT_EQUAL_MEM(expected, result, len)                                \
  _exam_assert_equal_mem(expected, result, len, __FILE__, __LINE__, __exam_env)

#define ASSERT_NOT_EQUAL_MEM(expected, result, len)                            \
  _exam_assert_not_equal_mem(expected, result, len, __FILE__, __LINE__,        \
                             __exam_env)

#endif // EXAMINER_H
