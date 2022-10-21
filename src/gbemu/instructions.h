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
    INC,
    DEC,
    ADD,
    SUB,
    XOR,
} MicroOperation;

typedef struct {
    BusIo io;
    Target ld;
    Target lhs;
    Target rhs;
    Target st;
    MicroOperation uop;
    bool halt;
    bool end;
} MicroInstr;

typedef MicroInstr Instruction[MICRO_INSTRUCTION_SIZE];

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep);

bool instructions_is_last_ustep(u8 opcode, u8 ustep);
