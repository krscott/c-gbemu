EXE := c-gbemu
TEST_EXE := test-$(EXE)
CC := gcc
BUILD_DIR := build
SRC_DIR := src/gbemu
SRC_MAIN := src/main.c
SRC_TEST := test/test.c

CFLAGS := -O2
LFLAGS := -Iinclude -Isrc -lmingw32 -lSDL2main -lSDL2

# Get all .c files
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# String substitution: src/hello.c -> build/src/hello.c.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
OBJ_MAIN := $(SRC_MAIN:%=$(BUILD_DIR)/%.o)
OBJ_TEST := $(SRC_TEST:%=$(BUILD_DIR)/%.o)

all: $(EXE) $(TEST_EXE)

$(EXE): $(OBJS) $(OBJ_MAIN)
	$(CC) $(OBJS) $(OBJ_MAIN) $(LFLAGS) -o $(EXE)

$(TEST_EXE): $(OBJS) $(OBJ_TEST)
	$(CC) $(OBJS) $(OBJ_TEST) $(LFLAGS) -o $(TEST_EXE)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LFLAGS) -c $< -o $@

check: $(TEST_EXE)
	$(TEST_EXE)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm $(EXE)
	rm $(TEST_EXE)
