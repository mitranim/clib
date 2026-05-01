# Shared stuff for inclusion in makefiles.

MAKEFLAGS := --silent
MAKE_CONC := $(MAKE) -j 128 CONC=true clear=$(or $(clear),false)
CLEAR ?= $(if $(filter false,$(clear)),, )
HERE ?= $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
CC ?= clang
PROD ?=
STRICT ?=
CFLAGS_DEBUG ?= -fsanitize=undefined,address,integer,nullability -fstack-protector
CFLAGS_PROD ?= -O2
DEBUG_FLAGS ?= -g3 $(if $(DEBUG),$(CFLAGS_DEBUG),$(if $(PROD),$(CFLAGS_PROD),$(CFLAGS_DEBUG)))
CRASH_FLAGS ?= $(and $(FAST_CRASH),-DFAST_CRASH)
STRICT_FLAGS ?= $(and $(STRICT),-Werror)
COMPILE_FLAGS ?= $(strip $(shell cat $(HERE)/compile_flags.txt))
CFLAGS ?= $(and $(PROD),-DPROD) $(COMPILE_FLAGS) $(STRICT_FLAGS) $(DEBUG_FLAGS) $(CRASH_FLAGS)
SRC_DIR ?= src
GEN_DIR ?= generated
ALL_SRC ?= $(shell find $(HERE) -type f \( -name '*.c' -or -name '*.h' \) )
FILE_EXE ?= $(and $(file),$(abspath $(basename $(file)).exe))
DISASM_FLAGS ?= --disassemble-all --headers --private-headers --reloc --dynamic-reloc --syms --dynamic-syms
WATCH_IGNORE ?= -i=$(GEN_DIR)
WATCH ?= watchexec $(and $(CLEAR),-c) $(WATCH_IGNORE) -r -d=1ms -n -q
WATCH_SRC ?= $(WATCH) -e=c,h,s
WATCH_EXE ?= $(WATCH) -e=exe --no-vcs-ignore
ARTIF ?= $(shell find . \( -type d -name '*.dSYM' \) -or \( -type f \( -name '.DS_Store' -or -name '*.o' -or -name '*.exe' \) \))

ifeq ($(verb),true)
	OK = echo [$@] ok
endif

# Disables some dangerous behaviors. Without this, `$@` sometimes changes from
# the intended target name to something surprising, like `makefile`, resulting
# in weird `cc` build commands that don't work and delete the wrong files.
.SUFFIXES:

# Auto-delete intermediary executables if any.
# Automatically affects `make run`.
.INTERMEDIATE: $(FILE_EXE)

# Example: `make some_file.exe && ./some_file.exe`. This recipe doesn't need
# to deal with multiple translation units or know dependencies between files,
# because we prefer "unity build": each C file includes all its dependencies
# via `#include` and uses `#pragma once` for deduplication.
#
# Even on Unix, using `.exe` is convenient. It makes this recipe
# possible, allows to use `.INTERMEDIATE` for auto-cleanup, and
# allows `make clean` to delete these executables by wildcard.
#
# Also see `make run` which runs and deletes the executable.
%.exe: %.c $(ALL_SRC)
	$(CC) $(CFLAGS) -x c $< -o $@
