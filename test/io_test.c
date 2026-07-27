#include "../src/io.c"
#include "./test.h"
#include <stddefer.h>
#include <unistd.h>

static void test_file_read_text_len_excludes_synthetic_nul() {
  char      path[] = "/tmp/clib_io_test_XXXXXX";
  const int fdes   = mkstemp(path);

  assert_fatal(fdes >= 0);
  defer close(fdes);
  defer unlink(path);

  static constexpr char body[] = "hello world";
  const Sint            wrote  = write(fdes, body, sizeof(body) - 1);

  test_eq_S64(wrote, sizeof(body) - 1);

  Uint  len = 0;
  char *out = nullptr;
  defer free(out);

  try_fatal(file_read_text(path, &out, &len));

  test_eq_U64(len, sizeof(body) - 1);
  test_eq_str(out, body);
}

static void test_file_read_text_rejects_non_regular_files() {
  Uint  len = 0;
  char *out = nullptr;
  defer free(out);

  const auto err = file_read_text("/dev/null", &out, &len);

  test_eq_str(err, "unable to read /dev/null: not a regular file");
  test_eq_U64(len, 0);
  test_assert(!out);
}

int main() {
  test_file_read_text_len_excludes_synthetic_nul();
  test_file_read_text_rejects_non_regular_files();
  return test_done();
}
