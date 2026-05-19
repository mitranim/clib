#include "../src/str.c"
#include "./test.h"

typedef str_buf(4) Small_str;

static void test_str_set_produces_null_terminated_buf(void) {
  Word_str word = {};

  const Err err = str_set(&word, "one");

  test_assert(!err);
  test_eq_U64(word.len, 3);
  test_eq_str(word.buf, "one");
  test_eq_U64(word.buf[word.len], '\0');
}

static void test_str_set_overflow_preserves_null_contract(void) {
  Small_str word = {};

  test_assert(!str_set(&word, "hi"));
  const Err err = str_set(&word, "abcd");

  test_assert(err);
  test_eq_U64(word.len, 3);
  test_eq_str(word.buf, "abc");
  test_eq_U64(word.buf[word.len], '\0');
}

static void test_str_push_preserves_null_termination(void) {
  Small_str word = {.buf = {'\0', 'x', 'x', 'x'}};

  test_assert(!str_push(&word, 'a'));
  test_eq_U64(word.len, 1);
  test_eq_U64(word.buf[word.len], '\0');
  if (!word.buf[word.len]) test_eq_str(word.buf, "a");

  test_assert(!str_push(&word, 'b'));
  test_eq_U64(word.len, 2);
  test_eq_U64(word.buf[word.len], '\0');
  if (!word.buf[word.len]) test_eq_str(word.buf, "ab");

  test_assert(str_push(&word, 'c'));
  test_eq_U64(word.len, 3);
  test_eq_U64(word.buf[word.len], '\0');
  if (!word.buf[word.len]) test_eq_str(word.buf, "abc");
}

static void test_str_trunc_preserves_null_termination(void) {
  Word_str word = {};

  test_assert(!str_set(&word, "abc"));
  str_trunc(&word);

  test_eq_U64(word.len, 0);
  test_eq_str(word.buf, "");
  test_eq_U64(word.buf[word.len], '\0');
}

static void test_str_fmt_preserves_null_termination(void) {
  Word_str word = {};

  str_fmt(&word, "%s %d", "abc", 123);

  test_eq_U64(word.len, strlen("abc 123"));
  test_eq_str(word.buf, "abc 123");
  test_eq_U64(word.buf[word.len], '\0');
}

static void test_str_eq_requires_exact_match(void) {
  Word_str word = {};

  const Err err = str_set(&word, "abc");
  test_assert(!err);
  if (err) return;

  test_assert(str_eq(&word, "abc"));
  test_assert(!str_eq(&word, "ab"));
  test_assert(!str_eq(&word, "abcd"));
}

int main(void) {
  test_str_set_produces_null_terminated_buf();
  test_str_set_overflow_preserves_null_contract();
  test_str_push_preserves_null_termination();
  test_str_trunc_preserves_null_termination();
  test_str_fmt_preserves_null_termination();
  test_str_eq_requires_exact_match();
  return test_done();
}
