# Using this weird incantation instead of `./` allows this include
# to work when the current file is included from other directories.
include $(abspath $(dir $(lastword $(MAKEFILE_LIST))))/make_misc.mk
-include .env.properties

MACH_GEN_SRC ?= mig/mach_exc.defs
MACH_GEN_OUT ?= $(GEN_DIR)/mach_exc.c
CODE_DIR ?= $(HOME)/code/$(USER)
SRC_DIR ?= src
TEST_DIR ?= test
SYNC_FLAGS ?= -au --itemize-changes $(and $(dry),-n)
TIMEOUT ?= gtimeout
TEST_TIMEOUT ?= 2
TEST_KILL_AFTER ?= 1
TEST_SRC ?= $(wildcard test/*_test.c)
TEST_EXE ?= $(TEST_SRC:.c=.exe)
FILES ?= $(shell find $(SRC_DIR) $(TEST_DIR) -type f \( -name '*.h' -or -name '*.c' \))

# Shut up pointless allocator warnings.
export MallocNanoZone=0

.PHONY: help
help: # Print help.
	echo "Select one of the following commands."
	echo "Viewing a definition: \`make -n <name>\`."
	echo
	for val in $(MAKEFILE_LIST); do \
		grep -E '^[[:alnum:]_]+(\.[[:alnum:]_]+)*:' $$val | sed 's/:.*#/#--/;s/:.*$$/#--/;s/^/  /' | column -t -s '#' | uniq || true; \
	done
	echo

.PHONY: vet
vet:
	clang-tidy --quiet $(FILES) -- $(CFLAGS) -ferror-limit=1
	$(OK)

.PHONY: vet_w
vet_w:
	$(WATCH_SRC) -- $(MAKE) vet

# The last extra newline is not cosmetic; it splits commands.
define RUN_TEST

echo [test] $(1)
$(TIMEOUT) --kill-after=$(TEST_KILL_AFTER)s $(TEST_TIMEOUT)s $(abspath $(1))

endef

.PHONY: test
test: $(TEST_EXE)
	$(foreach file,$(TEST_EXE),$(call RUN_TEST,$(file)))

.PHONY: test_w
test_w:
	$(WATCH_SRC) -- $(MAKE) test

# Usage example:
#
#   make run file=some_file.c
#   make run file=some_file.c args='one two three'
.PHONY: run
run: $(FILE_EXE)
	$(FILE_EXE) $(args)

.PHONY: run_w
run_w:
	$(WATCH_SRC) -- $(MAKE) run

# Fires off `lldb` with the given `file` and `args`
# without starting the executable.
.PHONY: debug
debug:
	lldb \
		--source-quietly \
		--one-line "settings set show-statusline false" \
		--one-line "settings set target.disable-aslr true" \
		$(and $(DEBUG),--one-line "settings set target.env-vars DEBUG=true") \
		--file $(file) -- $(args)

.PHONY: debug_run
debug_run:
	lldb --batch \
		--source-quietly \
		--one-line "settings set show-statusline false" \
		--one-line "settings set target.disable-aslr true" \
		$(and $(DEBUG),--one-line "settings set target.env-vars DEBUG=true") \
		--one-line "run" \
		--one-line "quit" \
		--file $(file) -- $(args)

.PHONY: debug_run_w
debug_run_w:
	$(WATCH_EXE) -- $(MAKE) debug_run

.PHONY: prepro
prepro:
	mkdir -p $(LOCAL)
	$(CC) -E $(CFLAGS) $(file) -o $(LOCAL)/out.c

.PHONY: asm
asm:
	mkdir -p $(LOCAL)
	$(CC) -S $(CFLAGS) $(file) -o $(LOCAL)/out.s

.PHONY: disasm
disasm:
	mkdir -p $(LOCAL)
	llvm-objdump $(DISASM_FLAGS) $(file) > $(LOCAL)/out.s

.PHONY: clean
clean:
	rm -rf $(GEN_DIR) $(wildcard $(ARTIF))

.PHONY: fmt
fmt:
	clang-format -i $(FILES)

# The MIG's output is much worse than this.
$(MACH_GEN_OUT): $(MACH_GEN_SRC)
	mkdir -p $(GEN_DIR)
	xcrun mig -server $(GEN_DIR)/tmp.c -user /dev/null -header /dev/null $(MACH_GEN_SRC) \
		&& cat mig/mach_pre.txt $(GEN_DIR)/tmp.c mig/mach_suf.txt \
			| sed -e 's/__attribute__((unused))//g' -e 's/__attribute__((__unused__))//g' \
			| clang-format \
			> $(MACH_GEN_OUT) \
		; rm -rf $(GEN_DIR)/tmp.c

# make get dir=some_repo/clib
.PHONY: get
get:
	rsync $(SYNC_FLAGS) "$(CODE_DIR)/$(dir)/" "./$(SRC_DIR)/"

# make set dir=some_repo/clib
.PHONY: set
set:
	rsync $(SYNC_FLAGS) "./$(SRC_DIR)/" "$(CODE_DIR)/$(dir)/"
