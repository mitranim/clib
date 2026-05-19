#include "../src/list.c"
#include "./test.h"

static void test_list_pop_returns_last_element(void) {
  deferred(list_deinit) list_of(U32) vals = {};

  list_append(&vals, (U32)11);
  list_append(&vals, (U32)22);

  test_eq_U64(list_pop(&vals), 22);
  test_eq_U64(vals.len, 1);
  test_eq_U64(list_pop(&vals), 11);
  test_eq_U64(vals.len, 0);
}

static void test_list_reserve_spare_cap_initializes_empty_list(void) {
  deferred(list_deinit) list_of(U32) vals = {};

  list_reserve_spare_cap(&vals, 3);

  test_assert(vals.dat);
  test_eq_U64(vals.len, 0);
  test_assert(vals.cap >= 3);
}

static void test_list_reserve_spare_cap_zero_does_not_allocate(void) {
  deferred(list_deinit) list_of(U32) vals = {};

  list_reserve_spare_cap(&vals, 0);

  test_assert(!vals.dat);
  test_eq_U64(vals.len, 0);
  test_eq_U64(vals.cap, 0);
}

int main(void) {
  test_list_pop_returns_last_element();
  test_list_reserve_spare_cap_initializes_empty_list();
  test_list_reserve_spare_cap_zero_does_not_allocate();
  return test_done();
}
