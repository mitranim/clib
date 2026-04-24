Small personal C library. Written while working on my [Astil Forth](https://github.com/mitranim/astil_forth).

Extracted for possible reuse, and to keep track of drafts which have no use in that project.

Assumes Clang. Only tested on MacOS. Some files include headers which don't exist on other platforms.

Assumes a "unity build". The `.c` files are meant to be used with `#include` or `#import` rather than compiled separately. Some macros in `.h` require procedure implementations from their `.c` file.

Changes between repos are synced via `make get` and `make set` commands.
