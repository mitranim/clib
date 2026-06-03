#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TEST_FAILURES = 0;

#define test_fail(fmt, ...)                                               \
  ({                                                                      \
    fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, __VA_ARGS__); \
    TEST_FAILURES++;                                                      \
  })

// // NOLINTBEGIN(bugprone-assignment-in-if-condition)
#define test_assert(expr)                                  \
  ({                                                       \
    if (!(expr)) test_fail("assertion failed: %s", #expr); \
  })
// NOLINTEND(bugprone-assignment-in-if-condition)

#define test_eq_U64(act, exp)                                                 \
  ({                                                                          \
    const auto test_act = (unsigned long long)(act);                          \
    const auto test_exp = (unsigned long long)(exp);                          \
    if (test_act != test_exp) {                                               \
      test_fail(                                                              \
        "expected %s == %s, got %llu vs %llu", #act, #exp, test_act, test_exp \
      );                                                                      \
    }                                                                         \
  })

#define test_eq_S64(act, exp)                                                 \
  ({                                                                          \
    const auto test_act = (long long)(act);                                   \
    const auto test_exp = (long long)(exp);                                   \
    if (test_act != test_exp) {                                               \
      test_fail(                                                              \
        "expected %s == %s, got %lld vs %lld", #act, #exp, test_act, test_exp \
      );                                                                      \
    }                                                                         \
  })

#define test_eq_str(act, exp)                                                 \
  ({                                                                          \
    const char *test_act = act;                                               \
    const char *test_exp = exp;                                               \
    if (strcmp(test_act, test_exp)) {                                         \
      test_fail(                                                              \
        "expected %s == %s, got `%s` vs `%s`", #act, #exp, test_act, test_exp \
      );                                                                      \
    }                                                                         \
  })

static int test_done() {
  if (TEST_FAILURES) {
    fprintf(stderr, "failed: %d\n", TEST_FAILURES);
    return 1;
  }
  return 0;
}
