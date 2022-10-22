#pragma once

#include "common.h"

#define MICRO_INSTRUCTION_SIZE 6

typedef enum {
    IO_NONE = 0,
    READ_PC_INC,
    READ_BC,
    READ_DE,
    READ_HL,
    READ_HL_INC,
    READ_HL_DEC,
    READ_SP_INC,
    READ_JP,
    READ_FF00_C,
    WRITE_BC,
    WRITE_DE,
    WRITE_HL,
    WRITE_HL_INC,
    WRITE_HL_DEC,
    WRITE_SP_DEC,
    WRITE_JP_INC,
    WRITE_FF00_C,
} BusIo;

typedef enum {
    TARGET_NONE = 0,
    A,
    F,
    B,
    C,
    D,
    E,
    H,
    L,
    BUS,
    SP_LO,
    SP_HI,
    PC_LO,
    PC_HI,
    JP_LO,
    JP_HI,
    IM_FF,
} Target;

typedef enum {
    UOP_NONE = 0,
    LD_R8_R8,
    INC16,
    DEC16,
    ADD16_LO,
    ADD16_HI,
    ADD16_SP_I8,
    ADD16_HL_SP_PLUS_I8,
    INC,
    DEC,
    ADD,
    ADC,
    SUB,
    SBC,
    AND,
    XOR,
    OR,
    CP,
    HALT,
    JP_REL,
    JP,
    JP_HL,
    RST,
    PREFIX_OP,
    RLCA,
    RRCA,
    RLA,
    RRA,
    DAA,
    CPL,
    SCF,
    CCF,
    DISABLE_INTERRUPTS,
    ENABLE_INTERRUPTS
} MicroOperation;

typedef enum {
    COND_ALWAYS = 0,
    COND_NZ,
    COND_Z,
    COND_NC,
    COND_C,
} Condition;

typedef struct {
    BusIo io;
    Target lhs;
    Target rhs;
    MicroOperation uop;
    Condition cond;
    bool end;
    bool undefined;
} MicroInstr;

typedef MicroInstr Instruction[MICRO_INSTRUCTION_SIZE];

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep);

bool instructions_is_last_ustep(u8 opcode, u8 ustep);
