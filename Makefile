EXE := c-gbemu
CC := gcc
BUILD_DIR := build
SRC_DIR := src

CFLAGS := -O2
LFLAGS := -Iinclude -lmingw32 -lSDL2main -lSDL2

# Get all .c files
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# String substitution: src/hello.c -> build/src/hello.c.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

$(EXE): $(OBJS)
	$(CC) $(OBJS) $(LFLAGS) -o $(EXE)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
	rm $(EXE)
