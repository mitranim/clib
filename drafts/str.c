/*
Arbitrarily-sized strings where zero termination is optional.

Many programs don't need a string abstraction of this level.
A combination of fixed-size buffers and constant strings is
often sufficient.
*/
#pragma once
#include "../src/err.h"
#include "../src/fmt.h"
#include "../src/num.h"

/*
If a C-string: `len < cap`; zero-terminated at `[len]`.
If not a C-string: `len <= cap`; zero-term is optional.
*/
typedef struct {
  char *buf;
  Ind   len;
  Ind   cap;
  void (*free)(void *); // For allocator-managed strings.
} Str;

static void buf_deinit(Str *val) {
  if (!val) return;
  const auto fun = val->free;
  if (fun && val->buf) fun(val->buf);
  *val = (Str){};
}

static bool buf_is_cstr(Str val) {
  return val.buf && val.cap > val.len && !val.buf[val.len];
}

static Str str_from(char *src) {
  if (!src) return (Str){};
  const auto len = strlen(src);
  return (Str){.buf = src, .len = len, .cap = len + 1};
}

static Ind str_char_at(Str str, char byte) {
  if (!str.buf) return INVALID_IND;
  const auto ptr = strchr(str.buf, byte);
  if (!ptr) return INVALID_IND;
  return ptr - str.buf;
}

static bool str_is_cli_flag(Str src) {
  return src.len && src.buf && src.buf[0] == '-' && src.len != 1;
}

static bool str_eq(Str one, Str two) {
  return one.len == two.len && !strncmp(one.buf, two.buf, one.len);
}

static Str str_copy(Str src) {
  if (!src.buf || !src.len) return (Str){};

  return (Str){
    .buf  = strndup(src.buf, src.len),
    .len  = src.len,
    .cap  = src.len + 1,
    .free = free,
  };
}

static void str_split(Str src, char byte, Str *one, Str *two) {
  *one = (Str){};
  *two = (Str){};

  const auto sep = src.buf ? strchr(src.buf, byte) : nullptr;

  if (!sep) {
    *one = src;
    return;
  }

  const auto ind = sep - src.buf;

  *one = (Str){
    .buf = src.buf,
    .len = ind,
    .cap = ind,
  };

  *two = (Str){
    .buf = sep + 1,
    .len = src.len - ind - 1,
    .cap = src.cap - ind - 1,
  };
}

static void cli_key_val(Str src, Str *key, Str *val) {
  *key = (Str){};
  *val = (Str){};

  if (!src.len) return;

  if (!str_is_cli_flag(src)) {
    *val = src;
    return;
  }

  str_split(src, '=', key, val);
}

int main(void) {
  char *argv_arr[] = {
    "",
    "one",
    "--two",
    "--three=",
    "--four=five",
  };
  int    argc = arr_cap(argv_arr);
  char **argv = argv_arr;

  const auto ceil = argv + argc;

  while (++argv < ceil) {
    const auto src = str_from(*argv);

    eprintf("src: ");
    repr_struct(&src);

    Str key;
    Str val;
    cli_key_val(src, &key, &val);

    eprintf("key: ");
    repr_struct(&key);
    eprintf("val: ");
    repr_struct(&val);

    defer(buf_deinit) Str copy = str_copy(src);
    (void)copy;
  }
}
