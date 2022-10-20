#pragma once

#include "common.h"

#define MICRO_INSTRUCTION_SIZE 6

typedef enum {
    IO_NONE = 0,
    FETCH_PC,
} BusIo;

typedef enum {
    TARGET_NONE = 0,
    A,
    B,
    C,
    D,
    E,
    H,
    L,
    TMP_LO,
    TMP_HI,
} Target;

typedef enum {
    UOP_NONE = 0,
    STORE_PC,
} MicroOperation;

typedef struct {
    BusIo io;
    Target dst;
    MicroOperation uop;
} MicroInstr;

typedef MicroInstr Instruction[MICRO_INSTRUCTION_SIZE];

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep);
