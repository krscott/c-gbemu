#include "instructions.h"

const Instruction instructions[0x100] = {
    // NOP
    [0x00] = {{0}},

    // JP u16
    [0xC3] = {{0},
              {.io = FETCH_PC, .dst = TMP_LO},
              {.io = FETCH_PC, .dst = TMP_HI},
              {.uop = STORE_PC}},
};

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep) {
    assert(ustep < MICRO_INSTRUCTION_SIZE);
    return &instructions[opcode][ustep];
}