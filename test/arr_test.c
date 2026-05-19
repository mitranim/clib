#include "../src/arr.h"
#include "../src/num.h"
#include "./test.h"

static void test_arr_copy_copies_all_bytes(void) {
  U32 src[]  = {0x11223344u, 0x55667788u, 0x99AABBCDu};
  U32 tar[3] = {};

  arr_copy(tar, src);

  test_eq_U64(tar[0], src[0]);
  test_eq_U64(tar[1], src[1]);
  test_eq_U64(tar[2], src[2]);
}

int main(void) {
  test_arr_copy_copies_all_bytes();
  return test_done();
}
