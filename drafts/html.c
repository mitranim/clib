#pragma once
#include "../src/list.c"
#include "../src/num.h"
#include <stdarg.h>
#include <stdio.h>

typedef struct Elem   Elem;
typedef list_of(Elem) Elems;

typedef struct Elem {
  const char *tag;
  const char *txt;
  Elems       chi;
  const char *cls;
} Elem;

#define E &(Elem)

#define elems(...) make_elems(__VA_ARGS__ __VA_OPT__(, ) nullptr)

static Elems make_elems(...) {
  va_list args;
  va_start(args);

  va_list count;
  va_copy(count, args);

  Ind len = 0;
  while (va_arg(count, Elem *)) len++;
  va_end(count);

  Elem *dat = malloc(sizeof(Elem) * len);

  Elems out = {
    .dat = dat,
    .cap = len,
    .len = len,
  };

  for (Ind ind = 0; ind < len; ind++) {
    *dat++ = *va_arg(args, Elem *);
  }

  va_end(args);
  return out;
}

static void elem_deinit(Elem *elem) {
  for (Ind ind = 0; ind < elem->chi.len; ind++) {
    elem_deinit(&elem->chi.dat[ind]);
  }
  list_deinit(&elem->chi);
}

static void elem_print_html(FILE *out, Elem *elem) {
  if (elem->tag) {
    fputc('<', out);
    fputs(elem->tag, out);

    if (elem->cls) {
      fputs(" class=", out);
      fputc('"', out);

      // Known issue: attr value needs escaping.
      fputs(elem->cls, out);

      fputc('"', out);
    }

    fputc('>', out);
  }

  // Known issue: needs escaping.
  if (elem->txt) fputs(elem->txt, out);

  for (Ind ind = 0; ind < elem->chi.len; ind++) {
    elem_print_html(out, &elem->chi.dat[ind]);
  }

  if (elem->tag) {
    fputc('<', out);
    fputc('/', out);
    fputs(elem->tag, out);
    fputc('>', out);
  }
}

int main(void) {
  const auto elem = E{
    .tag = "one",
    .cls = "three",
    .txt = "two",
    .chi = elems(
      E{.tag = "four", .txt = "five"},
      E{.tag = "six", .cls = "seven", .txt = "eight"},
      E{.tag = "nine", .txt = "ten"},
      E{.txt = "eleven"},
      E{.txt = "_"},
      E{.chi = elems(E{.chi = elems(E{.txt = "twelve"})})}
    ),
  };
  defer elem_deinit(elem);
  elem_print_html(stdout, elem);
  putc('\n', stdout);
}
