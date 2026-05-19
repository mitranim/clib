#include "../src/bits.c"
#include "./test.h"

static Bits expected_bits_span(U8 ceil) {
  Bits out = 0;
  for (U8 ind = 0; ind < ceil; ind++) out |= (Bits)1 << ind;
  return out;
}

// NOLINTBEGIN(bugprone-easily-swappable-parameters)
static Bits expected_bits_range(U8 floor, U8 ceil) {
  // NOLINTEND(bugprone-easily-swappable-parameters)

  Bits out = 0;
  for (U8 ind = floor; ind < ceil; ind++) out |= (Bits)1 << ind;
  return out;
}

static void test_bits_set_predicates_and_delete_all(void) {
  const Bits one   = 0b01010;
  const Bits two   = 0b10101;
  const Bits three = 0b00001;

  test_assert(!bits_has_some(one, two));
  test_assert(!bits_has_some(one, three));
  test_assert(bits_has_some(two, three));
  test_eq_U64(bits_del_all(three, one), three);
  test_eq_U64(bits_del_all(three, two), 0);
}

static void test_bits_pop_low_returns_low_set_bits_and_clears_them(void) {
  Bits set = 0b0111000111000;

  test_eq_U64(bits_pop_low(&set), 3);
  test_eq_U64(set, 0b0111000110000);
  test_eq_U64(bits_pop_low(&set), 4);
  test_eq_U64(bits_pop_low(&set), 5);
  test_eq_U64(bits_pop_low(&set), 9);
  test_eq_U64(bits_pop_low(&set), 10);
  test_eq_U64(bits_pop_low(&set), 11);
  test_eq_U64(set, 0);
}

static void test_bits_push_low_returns_low_clear_bits_and_sets_them(void) {
  const Bits src = 0b0111000111000;
  Bits       set = src;

  for (U8 bit = 0; bit < BITS_CEIL; bit++) {
    if (bits_has(src, bit)) continue;
    test_eq_U64(bits_push_low(&set), bit);
    test_assert(bits_has(set, bit));
  }
  test_assert(bits_full(set));
}

static void test_bits_span_sets_low_bits(void) {
  test_eq_U64(bits_span(0), 0);
  test_eq_U64(bits_span(5), expected_bits_span(5));
  test_eq_U64(bits_span(63), expected_bits_span(63));
  test_eq_U64(bits_span(64), expected_bits_span(64));
}

static void test_bits_range_sets_half_open_range(void) {
  test_eq_U64(bits_range(5, 9), expected_bits_range(5, 9));
  test_eq_U64(bits_range(0, 23), expected_bits_range(0, 23));
  test_eq_U64(bits_range(0, 31), expected_bits_range(0, 31));
  test_eq_U64(bits_range(0, 32), expected_bits_range(0, 32));
  test_eq_U64(bits_range(0, 40), expected_bits_range(0, 40));
  test_eq_U64(bits_range(0, 51), expected_bits_range(0, 51));
  test_eq_U64(bits_range(51, 62), expected_bits_range(51, 62));
  test_eq_U64(bits_range(51, 63), expected_bits_range(51, 63));
  test_eq_U64(bits_range(51, 64), expected_bits_range(51, 64));
}

static void test_bits_add_span_sets_low_bits(void) {
  test_eq_U64(
    bits_add_span((Bits)1 << 5u, 4), ((Bits)1 << 5u) | expected_bits_span(4)
  );
}

static void test_bits_del_span_clears_low_bits(void) {
  const Bits set = ((Bits)1 << 5u) | ((Bits)1 << 3u);

  test_eq_U64(bits_del_span(set, 4), ((Bits)1 << 5u));
}

int main(void) {
  test_bits_set_predicates_and_delete_all();
  test_bits_pop_low_returns_low_set_bits_and_clears_them();
  test_bits_push_low_returns_low_clear_bits_and_sets_them();
  test_bits_span_sets_low_bits();
  test_bits_range_sets_half_open_range();
  test_bits_add_span_sets_low_bits();
  test_bits_del_span_clears_low_bits();
  return test_done();
}
