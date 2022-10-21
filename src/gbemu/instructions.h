#pragma once

#include "common.h"

#define MICRO_INSTRUCTION_SIZE 6

typedef enum {
    IO_NONE = 0,
    READ_PC,
    WRITE_HL,
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
    BUS,
    TMP_LO,
    TMP_HI,
} Target;

typedef enum {
    UOP_NONE = 0,
    LD_BUS_LHS,
    LD_PC_TMP,
    INC,
    DEC,
    ADD,
    ADC,
    SUB,
    SBC,
    XOR,
    HALT,
} MicroOperation;

typedef struct {
    BusIo io;
    Target ld;
    Target lhs;
    Target rhs;
    MicroOperation uop;
    bool end;
    bool undefined;
} MicroInstr;

typedef MicroInstr Instruction[MICRO_INSTRUCTION_SIZE];

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep);

bool instructions_is_last_ustep(u8 opcode, u8 ustep);
