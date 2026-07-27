#include "../src/stack.c"
#include "./test.h"

static void test_stack_pop_works_when_full() {
  deferred(stack_deinit) stack_of(U32) vals = {};

  Stack_opt opt = {.len = 2};

  try_fatal(stack_init(&vals, &opt));
  test_eq_U64(stack_len(&vals), 0);

  stack_push(&vals, (U32)11);
  test_eq_U64(stack_len(&vals), 1);

  stack_push(&vals, (U32)22);
  test_eq_U64(stack_len(&vals), 2);

  test_eq_U64(stack_pop(&vals), 22);
  test_eq_U64(stack_pop(&vals), 11);
  test_eq_U64(stack_len(&vals), 0);
}

int main() {
  test_stack_pop_works_when_full();
  return test_done();
}
