#include "instructions.h"

// Instructions based on https://izik1.github.io/gbops/

const Instruction instructions[0x100] = {
    // NOP
    [0x00] = {{.end = true}},

    // INC B
    [0x04] = {{.lhs = B, .uop = INC, .st = B, .end = true}},

    // DEC B
    [0x05] = {{.lhs = B, .uop = DEC, .st = B, .end = true}},

    // LD B,u8
    [0x06] = {{0}, {.io = FETCH_PC, .ld = B, .end = true}},

    // LD A,u8
    [0x3E] = {{0}, {.io = FETCH_PC, .ld = A, .end = true}},

    // HALT
    [0x76] = {{.halt = true, .end = true}},

    // XOR A,B
    [0xA8] = {{.lhs = A, .rhs = B, .uop = XOR, .st = A, .end = true}},

    // XOR A,A
    [0xAF] = {{.lhs = A, .rhs = A, .uop = XOR, .st = A, .end = true}},

    // JP u16
    [0xC3] = {{0},
              {.io = FETCH_PC, .ld = TMP_LO},
              {.io = FETCH_PC, .ld = TMP_HI},
              {.uop = STORE_PC, .end = true}},
};

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep) {
    assert(ustep < MICRO_INSTRUCTION_SIZE);
    return &instructions[opcode][ustep];
}

bool instructions_is_last_ustep(u8 opcode, u8 ustep) {
    if (ustep >= MICRO_INSTRUCTION_SIZE - 1) return true;
    return instructions[opcode][ustep].end;
}