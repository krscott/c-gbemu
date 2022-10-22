EXE := c-gbemu
TEST_EXE := test-$(EXE)
CC := gcc
BUILD_DIR := build
SRC_DIR := src/gbemu
SRC_MAIN := src/main.c
SRC_TEST := test/test.c

BUILD := debug

cflags.debug := -ggdb -static-libgcc
cflags.release := -O3

CFLAGS := -Wall -Wextra -Wjump-misses-init -Werror -std=c17 ${cflags.${BUILD}}
LFLAGS := -Iinclude -Isrc -lmingw32 -lSDL2main -lSDL2

# Get all .c files
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# String substitution: src/hello.c -> build/src/hello.c.debug.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.$(BUILD).o)
OBJ_MAIN := $(SRC_MAIN:%=$(BUILD_DIR)/%.$(BUILD).o)
OBJ_TEST := $(SRC_TEST:%=$(BUILD_DIR)/%.$(BUILD).o)

all: $(EXE) $(TEST_EXE)

$(EXE): $(OBJS) $(OBJ_MAIN)
	$(CC) $(OBJS) $(OBJ_MAIN) $(LFLAGS) -o $(EXE)

$(TEST_EXE): $(OBJS) $(OBJ_TEST)
	$(CC) $(OBJS) $(OBJ_TEST) $(LFLAGS) -o $(TEST_EXE)

$(BUILD_DIR)/%.c.$(BUILD).o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LFLAGS) -c $< -o $@

# On github windows CI, need to call full filename with '.exe'
check: $(EXE) $(TEST_EXE)
	$(shell find . -name '$(TEST_EXE)*')

drmem: $(TEST_EXE)
	drmemory -batch -suppress drmem-suppress.txt -- $(TEST_EXE)

.PHONY: clean
clean:
	@[ -d $(BUILD_DIR) ] && rm -r $(BUILD_DIR) || true
	@[ -f $(EXE) ] && rm $(EXE) || true
	@[ -f $(TEST_EXE) ] && rm $(TEST_EXE) || true
