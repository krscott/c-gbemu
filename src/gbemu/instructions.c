#include "instructions.h"

// Instructions based on https://izik1.github.io/gbops/

const Instruction instructions[0x100] = {
    [0x00] = {{.end = true}},
    [0x01] = {{0},
              {.uop = LD_R8_R8, .lhs = C, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = B,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x02] = {{0},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = A,
               .io = WRITE_HL_INC,
               .end = true}},
    [0x03] = {{.uop = INC16_LO, .lhs = B, .rhs = C},
              {.uop = INC16_HI, .lhs = B, .rhs = C, .end = true}},
    [0x04] = {{.uop = INC, .lhs = B, .end = true}},
    [0x05] = {{.uop = DEC, .lhs = B, .end = true}},
    [0x06] = {{0},
              {.uop = LD_R8_R8,
               .lhs = B,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x07] = {{.undefined = true}},
    [0x08] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = SP_LO, .io = WRITE_JP_INC},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = SP_HI,
               .io = WRITE_JP_INC,
               .end = true}},
    [0x09] = {{.uop = ADD16_LO, .lhs = L, .rhs = C},
              {.uop = ADD16_HI, .lhs = H, .rhs = B, .end = true}},
    [0x0A] = {{0},
              {.uop = LD_R8_R8,
               .lhs = A,
               .rhs = BUS,
               .io = READ_HL_INC,
               .end = true}},
    [0x0B] = {{.uop = DEC16_LO, .lhs = B, .rhs = C},
              {.uop = DEC16_HI, .lhs = B, .rhs = C, .end = true}},
    [0x0C] = {{.uop = INC, .lhs = C, .end = true}},
    [0x0D] = {{.uop = DEC, .lhs = C, .end = true}},
    [0x0E] = {{0},
              {.uop = LD_R8_R8,
               .lhs = C,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x0F] = {{.undefined = true}},
    [0x10] = {{0},
              {.uop = LD_R8_R8,
               .lhs = JP_LO,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_Z},
              {.uop = JP_REL, .end = true}},
    [0x11] = {{0},
              {.uop = LD_R8_R8, .lhs = E, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = D,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x12] = {{0},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = A,
               .io = WRITE_HL_INC,
               .end = true}},
    [0x13] = {{.uop = INC16_LO, .lhs = D, .rhs = E},
              {.uop = INC16_HI, .lhs = D, .rhs = E, .end = true}},
    [0x14] = {{.uop = INC, .lhs = D, .end = true}},
    [0x15] = {{.uop = DEC, .lhs = D, .end = true}},
    [0x16] = {{0},
              {.uop = LD_R8_R8,
               .lhs = D,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x17] = {{.undefined = true}},
    [0x18] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = JP_REL, .end = true}},
    [0x19] = {{.uop = ADD16_LO, .lhs = L, .rhs = E},
              {.uop = ADD16_HI, .lhs = H, .rhs = D, .end = true}},
    [0x1A] = {{0},
              {.uop = LD_R8_R8,
               .lhs = A,
               .rhs = BUS,
               .io = READ_HL_INC,
               .end = true}},
    [0x1B] = {{.uop = DEC16_LO, .lhs = D, .rhs = E},
              {.uop = DEC16_HI, .lhs = D, .rhs = E, .end = true}},
    [0x1C] = {{.uop = INC, .lhs = E, .end = true}},
    [0x1D] = {{.uop = DEC, .lhs = E, .end = true}},
    [0x1E] = {{0},
              {.uop = LD_R8_R8,
               .lhs = E,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x1F] = {{.undefined = true}},
    [0x20] = {{0},
              {.uop = LD_R8_R8,
               .lhs = JP_LO,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_NC},
              {.uop = JP_REL, .end = true}},
    [0x21] = {{0},
              {.uop = LD_R8_R8, .lhs = L, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = H,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x22] = {{0},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = A,
               .io = WRITE_HL_INC,
               .end = true}},
    [0x23] = {{.uop = INC16_LO, .lhs = H, .rhs = L},
              {.uop = INC16_HI, .lhs = H, .rhs = L, .end = true}},
    [0x24] = {{.uop = INC, .lhs = H, .end = true}},
    [0x25] = {{.uop = DEC, .lhs = H, .end = true}},
    [0x26] = {{0},
              {.uop = LD_R8_R8,
               .lhs = H,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x27] = {{.undefined = true}},
    [0x28] = {{0},
              {.uop = LD_R8_R8,
               .lhs = JP_LO,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_NC},
              {.uop = JP_REL, .end = true}},
    [0x29] = {{.uop = ADD16_LO, .lhs = L, .rhs = L},
              {.uop = ADD16_HI, .lhs = H, .rhs = H, .end = true}},
    [0x2A] = {{0},
              {.uop = LD_R8_R8,
               .lhs = A,
               .rhs = BUS,
               .io = READ_HL_INC,
               .end = true}},
    [0x2B] = {{.uop = DEC16_LO, .lhs = H, .rhs = L},
              {.uop = DEC16_HI, .lhs = H, .rhs = L, .end = true}},
    [0x2C] = {{.uop = INC, .lhs = L, .end = true}},
    [0x2D] = {{.uop = DEC, .lhs = L, .end = true}},
    [0x2E] = {{0},
              {.uop = LD_R8_R8,
               .lhs = L,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x2F] = {{.undefined = true}},
    [0x30] = {{0},
              {.uop = LD_R8_R8,
               .lhs = JP_LO,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_C},
              {.uop = JP_REL, .end = true}},
    [0x31] = {{0},
              {.uop = LD_R8_R8, .lhs = SP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = SP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x32] = {{0},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = A,
               .io = WRITE_HL_INC,
               .end = true}},
    [0x33] = {{.uop = INC16_LO, .lhs = SP_HI, .rhs = SP_LO},
              {.uop = INC16_HI, .lhs = SP_HI, .rhs = SP_LO, .end = true}},
    [0x34] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .io = READ_HL},
              {.uop = INC, .lhs = BUS, .io = WRITE_HL, .end = true}},
    [0x35] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .io = READ_HL},
              {.uop = DEC, .lhs = BUS, .io = WRITE_HL, .end = true}},
    [0x36] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8, .lhs = BUS, .io = WRITE_HL, .end = true}},
    [0x37] = {{.undefined = true}},
    [0x38] = {{0},
              {.uop = LD_R8_R8,
               .lhs = JP_LO,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_C},
              {.uop = JP_REL, .end = true}},
    [0x39] = {{.uop = ADD16_LO, .lhs = L, .rhs = SP_LO},
              {.uop = ADD16_HI, .lhs = H, .rhs = SP_HI, .end = true}},
    [0x3A] = {{0},
              {.uop = LD_R8_R8,
               .lhs = A,
               .rhs = BUS,
               .io = READ_HL_INC,
               .end = true}},
    [0x3B] = {{.uop = DEC16_LO, .lhs = SP_HI, .rhs = SP_LO},
              {.uop = DEC16_HI, .lhs = SP_HI, .rhs = SP_LO, .end = true}},
    [0x3C] = {{.uop = INC, .lhs = A, .end = true}},
    [0x3D] = {{.uop = DEC, .lhs = A, .end = true}},
    [0x3E] = {{0},
              {.uop = LD_R8_R8,
               .lhs = A,
               .rhs = BUS,
               .io = READ_PC_INC,
               .end = true}},
    [0x3F] = {{.undefined = true}},
    [0x40] = {{.uop = LD_R8_R8, .lhs = B, .rhs = B, .end = true}},
    [0x41] = {{.uop = LD_R8_R8, .lhs = B, .rhs = C, .end = true}},
    [0x42] = {{.uop = LD_R8_R8, .lhs = B, .rhs = D, .end = true}},
    [0x43] = {{.uop = LD_R8_R8, .lhs = B, .rhs = E, .end = true}},
    [0x44] = {{.uop = LD_R8_R8, .lhs = B, .rhs = H, .end = true}},
    [0x45] = {{.uop = LD_R8_R8, .lhs = B, .rhs = L, .end = true}},
    [0x46] =
        {{0},
         {.uop = LD_R8_R8, .lhs = B, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x47] = {{.uop = LD_R8_R8, .lhs = B, .rhs = A, .end = true}},
    [0x48] = {{.uop = LD_R8_R8, .lhs = C, .rhs = B, .end = true}},
    [0x49] = {{.uop = LD_R8_R8, .lhs = C, .rhs = C, .end = true}},
    [0x4A] = {{.uop = LD_R8_R8, .lhs = C, .rhs = D, .end = true}},
    [0x4B] = {{.uop = LD_R8_R8, .lhs = C, .rhs = E, .end = true}},
    [0x4C] = {{.uop = LD_R8_R8, .lhs = C, .rhs = H, .end = true}},
    [0x4D] = {{.uop = LD_R8_R8, .lhs = C, .rhs = L, .end = true}},
    [0x4E] =
        {{0},
         {.uop = LD_R8_R8, .lhs = C, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x4F] = {{.uop = LD_R8_R8, .lhs = C, .rhs = A, .end = true}},
    [0x50] = {{.uop = LD_R8_R8, .lhs = D, .rhs = B, .end = true}},
    [0x51] = {{.uop = LD_R8_R8, .lhs = D, .rhs = C, .end = true}},
    [0x52] = {{.uop = LD_R8_R8, .lhs = D, .rhs = D, .end = true}},
    [0x53] = {{.uop = LD_R8_R8, .lhs = D, .rhs = E, .end = true}},
    [0x54] = {{.uop = LD_R8_R8, .lhs = D, .rhs = H, .end = true}},
    [0x55] = {{.uop = LD_R8_R8, .lhs = D, .rhs = L, .end = true}},
    [0x56] =
        {{0},
         {.uop = LD_R8_R8, .lhs = D, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x57] = {{.uop = LD_R8_R8, .lhs = D, .rhs = A, .end = true}},
    [0x58] = {{.uop = LD_R8_R8, .lhs = E, .rhs = B, .end = true}},
    [0x59] = {{.uop = LD_R8_R8, .lhs = E, .rhs = C, .end = true}},
    [0x5A] = {{.uop = LD_R8_R8, .lhs = E, .rhs = D, .end = true}},
    [0x5B] = {{.uop = LD_R8_R8, .lhs = E, .rhs = E, .end = true}},
    [0x5C] = {{.uop = LD_R8_R8, .lhs = E, .rhs = H, .end = true}},
    [0x5D] = {{.uop = LD_R8_R8, .lhs = E, .rhs = L, .end = true}},
    [0x5E] =
        {{0},
         {.uop = LD_R8_R8, .lhs = E, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x5F] = {{.uop = LD_R8_R8, .lhs = E, .rhs = A, .end = true}},
    [0x60] = {{.uop = LD_R8_R8, .lhs = H, .rhs = B, .end = true}},
    [0x61] = {{.uop = LD_R8_R8, .lhs = H, .rhs = C, .end = true}},
    [0x62] = {{.uop = LD_R8_R8, .lhs = H, .rhs = D, .end = true}},
    [0x63] = {{.uop = LD_R8_R8, .lhs = H, .rhs = E, .end = true}},
    [0x64] = {{.uop = LD_R8_R8, .lhs = H, .rhs = H, .end = true}},
    [0x65] = {{.uop = LD_R8_R8, .lhs = H, .rhs = L, .end = true}},
    [0x66] =
        {{0},
         {.uop = LD_R8_R8, .lhs = H, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x67] = {{.uop = LD_R8_R8, .lhs = H, .rhs = A, .end = true}},
    [0x68] = {{.uop = LD_R8_R8, .lhs = L, .rhs = B, .end = true}},
    [0x69] = {{.uop = LD_R8_R8, .lhs = L, .rhs = C, .end = true}},
    [0x6A] = {{.uop = LD_R8_R8, .lhs = L, .rhs = D, .end = true}},
    [0x6B] = {{.uop = LD_R8_R8, .lhs = L, .rhs = E, .end = true}},
    [0x6C] = {{.uop = LD_R8_R8, .lhs = L, .rhs = H, .end = true}},
    [0x6D] = {{.uop = LD_R8_R8, .lhs = L, .rhs = L, .end = true}},
    [0x6E] =
        {{0},
         {.uop = LD_R8_R8, .lhs = L, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x6F] = {{.uop = LD_R8_R8, .lhs = L, .rhs = A, .end = true}},
    [0x70] =
        {{0},
         {.uop = LD_R8_R8, .lhs = BUS, .rhs = B, .io = WRITE_HL, .end = true}},
    [0x71] =
        {{0},
         {.uop = LD_R8_R8, .lhs = BUS, .rhs = C, .io = WRITE_HL, .end = true}},
    [0x72] =
        {{0},
         {.uop = LD_R8_R8, .lhs = BUS, .rhs = D, .io = WRITE_HL, .end = true}},
    [0x73] =
        {{0},
         {.uop = LD_R8_R8, .lhs = BUS, .rhs = E, .io = WRITE_HL, .end = true}},
    [0x74] =
        {{0},
         {.uop = LD_R8_R8, .lhs = BUS, .rhs = H, .io = WRITE_HL, .end = true}},
    [0x75] =
        {{0},
         {.uop = LD_R8_R8, .lhs = BUS, .rhs = L, .io = WRITE_HL, .end = true}},
    [0x76] = {{.uop = HALT, .end = true}},
    [0x77] =
        {{0},
         {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_HL, .end = true}},
    [0x78] = {{.uop = LD_R8_R8, .lhs = A, .rhs = B, .end = true}},
    [0x79] = {{.uop = LD_R8_R8, .lhs = A, .rhs = C, .end = true}},
    [0x7A] = {{.uop = LD_R8_R8, .lhs = A, .rhs = D, .end = true}},
    [0x7B] = {{.uop = LD_R8_R8, .lhs = A, .rhs = E, .end = true}},
    [0x7C] = {{.uop = LD_R8_R8, .lhs = A, .rhs = H, .end = true}},
    [0x7D] = {{.uop = LD_R8_R8, .lhs = A, .rhs = L, .end = true}},
    [0x7E] =
        {{0},
         {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x7F] = {{.uop = LD_R8_R8, .lhs = A, .rhs = A, .end = true}},
    [0x80] = {{.uop = ADD, .lhs = A, .rhs = B, .end = true}},
    [0x81] = {{.uop = ADD, .lhs = A, .rhs = C, .end = true}},
    [0x82] = {{.uop = ADD, .lhs = A, .rhs = D, .end = true}},
    [0x83] = {{.uop = ADD, .lhs = A, .rhs = E, .end = true}},
    [0x84] = {{.uop = ADD, .lhs = A, .rhs = H, .end = true}},
    [0x85] = {{.uop = ADD, .lhs = A, .rhs = L, .end = true}},
    [0x86] = {{0},
              {.uop = ADD, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x87] = {{.uop = ADD, .lhs = A, .rhs = A, .end = true}},
    [0x88] = {{.uop = ADC, .lhs = A, .rhs = B, .end = true}},
    [0x89] = {{.uop = ADC, .lhs = A, .rhs = C, .end = true}},
    [0x8A] = {{.uop = ADC, .lhs = A, .rhs = D, .end = true}},
    [0x8B] = {{.uop = ADC, .lhs = A, .rhs = E, .end = true}},
    [0x8C] = {{.uop = ADC, .lhs = A, .rhs = H, .end = true}},
    [0x8D] = {{.uop = ADC, .lhs = A, .rhs = L, .end = true}},
    [0x8E] = {{0},
              {.uop = ADC, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x8F] = {{.uop = ADC, .lhs = A, .rhs = A, .end = true}},
    [0x90] = {{.uop = SUB, .lhs = A, .rhs = B, .end = true}},
    [0x91] = {{.uop = SUB, .lhs = A, .rhs = C, .end = true}},
    [0x92] = {{.uop = SUB, .lhs = A, .rhs = D, .end = true}},
    [0x93] = {{.uop = SUB, .lhs = A, .rhs = E, .end = true}},
    [0x94] = {{.uop = SUB, .lhs = A, .rhs = H, .end = true}},
    [0x95] = {{.uop = SUB, .lhs = A, .rhs = L, .end = true}},
    [0x96] = {{0},
              {.uop = SUB, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x97] = {{.uop = SUB, .lhs = A, .rhs = A, .end = true}},
    [0x98] = {{.uop = SBC, .lhs = A, .rhs = B, .end = true}},
    [0x99] = {{.uop = SBC, .lhs = A, .rhs = C, .end = true}},
    [0x9A] = {{.uop = SBC, .lhs = A, .rhs = D, .end = true}},
    [0x9B] = {{.uop = SBC, .lhs = A, .rhs = E, .end = true}},
    [0x9C] = {{.uop = SBC, .lhs = A, .rhs = H, .end = true}},
    [0x9D] = {{.uop = SBC, .lhs = A, .rhs = L, .end = true}},
    [0x9E] = {{0},
              {.uop = SBC, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0x9F] = {{.uop = SBC, .lhs = A, .rhs = A, .end = true}},
    [0xA0] = {{.uop = AND, .lhs = A, .rhs = B, .end = true}},
    [0xA1] = {{.uop = AND, .lhs = A, .rhs = C, .end = true}},
    [0xA2] = {{.uop = AND, .lhs = A, .rhs = D, .end = true}},
    [0xA3] = {{.uop = AND, .lhs = A, .rhs = E, .end = true}},
    [0xA4] = {{.uop = AND, .lhs = A, .rhs = H, .end = true}},
    [0xA5] = {{.uop = AND, .lhs = A, .rhs = L, .end = true}},
    [0xA6] = {{0},
              {.uop = AND, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0xA7] = {{.uop = AND, .lhs = A, .rhs = A, .end = true}},
    [0xA8] = {{.uop = XOR, .lhs = A, .rhs = B, .end = true}},
    [0xA9] = {{.uop = XOR, .lhs = A, .rhs = C, .end = true}},
    [0xAA] = {{.uop = XOR, .lhs = A, .rhs = D, .end = true}},
    [0xAB] = {{.uop = XOR, .lhs = A, .rhs = E, .end = true}},
    [0xAC] = {{.uop = XOR, .lhs = A, .rhs = H, .end = true}},
    [0xAD] = {{.uop = XOR, .lhs = A, .rhs = L, .end = true}},
    [0xAE] = {{0},
              {.uop = XOR, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0xAF] = {{.uop = XOR, .lhs = A, .rhs = A, .end = true}},
    [0xB0] = {{.uop = OR, .lhs = A, .rhs = B, .end = true}},
    [0xB1] = {{.uop = OR, .lhs = A, .rhs = C, .end = true}},
    [0xB2] = {{.uop = OR, .lhs = A, .rhs = D, .end = true}},
    [0xB3] = {{.uop = OR, .lhs = A, .rhs = E, .end = true}},
    [0xB4] = {{.uop = OR, .lhs = A, .rhs = H, .end = true}},
    [0xB5] = {{.uop = OR, .lhs = A, .rhs = L, .end = true}},
    [0xB6] = {{0},
              {.uop = OR, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0xB7] = {{.uop = OR, .lhs = A, .rhs = A, .end = true}},
    [0xB8] = {{.uop = CP, .lhs = A, .rhs = B, .end = true}},
    [0xB9] = {{.uop = CP, .lhs = A, .rhs = C, .end = true}},
    [0xBA] = {{.uop = CP, .lhs = A, .rhs = D, .end = true}},
    [0xBB] = {{.uop = CP, .lhs = A, .rhs = E, .end = true}},
    [0xBC] = {{.uop = CP, .lhs = A, .rhs = H, .end = true}},
    [0xBD] = {{.uop = CP, .lhs = A, .rhs = L, .end = true}},
    [0xBE] = {{0},
              {.uop = CP, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}},
    [0xBF] = {{.uop = CP, .lhs = A, .rhs = A, .end = true}},
    [0xC0] = {{0},
              {.cond = COND_NZ},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
              {.uop = JP, .end = true}},
    [0xC1] = {{0},
              {.uop = LD_R8_R8, .lhs = C, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8,
               .lhs = B,
               .rhs = BUS,
               .io = READ_SP_INC,
               .end = true}},
    [0xC2] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_NZ},
              {.uop = JP, .end = true}},
    [0xC3] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_ALWAYS},
              {.uop = JP, .end = true}},
    [0xC4] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_NZ},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = JP, .end = true}},
    [0xC5] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = C, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = B,
               .io = WRITE_SP_DEC,
               .end = true}},
    [0xC6] =
        {{0},
         {.uop = ADD, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xC7] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
    [0xC8] = {{0},
              {.cond = COND_NZ},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
              {.uop = JP, .end = true}},
    [0xC9] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
              {.uop = JP, .end = true}},
    [0xCA] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_NZ},
              {.uop = JP, .end = true}},
    [0xCB] = {{0}, {.uop = PREFIX_OP, .io = READ_PC_INC, .end = true}},
    [0xCC] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_NZ},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = JP, .end = true}},
    [0xCD] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_ALWAYS},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = JP, .end = true}},
    [0xCE] =
        {{0},
         {.uop = ADC, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xCF] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
    [0xD0] = {{0},
              {.cond = COND_Z},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
              {.uop = JP, .end = true}},
    [0xD1] = {{0},
              {.uop = LD_R8_R8, .lhs = E, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8,
               .lhs = D,
               .rhs = BUS,
               .io = READ_SP_INC,
               .end = true}},
    [0xD2] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_Z},
              {.uop = JP, .end = true}},
    [0xD3] = {{.undefined = true, .end = true}},
    [0xD4] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_Z},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = JP, .end = true}},
    [0xD5] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = E, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = D,
               .io = WRITE_SP_DEC,
               .end = true}},
    [0xD6] =
        {{0},
         {.uop = SUB, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xD7] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
    [0xD8] = {{0},
              {.cond = COND_Z},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
              {.uop = JP, .end = true}},
    [0xD9] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
              {.uop = JP, .end = true}},
    [0xDA] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_Z},
              {.uop = JP, .end = true}},
    [0xDB] = {{.undefined = true, .end = true}},
    [0xDC] = {{0},
              {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
              {.uop = LD_R8_R8,
               .lhs = JP_HI,
               .rhs = BUS,
               .io = READ_PC_INC,
               .cond = COND_Z},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = JP, .end = true}},
    [0xDD] = {{.undefined = true, .end = true}},
    [0xDE] =
        {{0},
         {.uop = SBC, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xDF] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
    [0xE0] = {{.undefined = true}},
    [0xE1] = {{0},
              {.uop = LD_R8_R8, .lhs = L, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8,
               .lhs = H,
               .rhs = BUS,
               .io = READ_SP_INC,
               .end = true}},
    [0xE2] = {{.undefined = true}},
    [0xE3] = {{.undefined = true, .end = true}},
    [0xE4] = {{.undefined = true, .end = true}},
    [0xE5] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = L, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = H,
               .io = WRITE_SP_DEC,
               .end = true}},
    [0xE6] =
        {{0},
         {.uop = AND, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xE7] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
    [0xE8] = {{.undefined = true}},
    [0xE9] = {{.uop = JP_HL, .end = true}},
    [0xEA] = {{.undefined = true}},
    [0xEB] = {{.undefined = true, .end = true}},
    [0xEC] = {{.undefined = true, .end = true}},
    [0xED] = {{.undefined = true, .end = true}},
    [0xEE] =
        {{0},
         {.uop = XOR, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xEF] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
    [0xF0] = {{.undefined = true}},
    [0xF1] = {{0},
              {.uop = LD_R8_R8, .lhs = F, .rhs = BUS, .io = READ_SP_INC},
              {.uop = LD_R8_R8,
               .lhs = A,
               .rhs = BUS,
               .io = READ_SP_INC,
               .end = true}},
    [0xF2] = {{.undefined = true}},
    [0xF3] = {{.undefined = true}},
    [0xF4] = {{.undefined = true, .end = true}},
    [0xF5] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = F, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8,
               .lhs = BUS,
               .rhs = A,
               .io = WRITE_SP_DEC,
               .end = true}},
    [0xF6] =
        {{0},
         {.uop = OR, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xF7] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
    [0xF8] = {{.undefined = true}},
    [0xF9] = {{.undefined = true}},
    [0xFA] = {{.undefined = true}},
    [0xFB] = {{.undefined = true}},
    [0xFC] = {{.undefined = true, .end = true}},
    [0xFD] = {{.undefined = true, .end = true}},
    [0xFE] =
        {{0},
         {.uop = CP, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}},
    [0xFF] = {{0},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
              {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
              {.uop = RST, .end = true}},
};

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep) {
    assert(ustep < MICRO_INSTRUCTION_SIZE);
    return &instructions[opcode][ustep];
}

bool instructions_is_last_ustep(u8 opcode, u8 ustep) {
    if (ustep >= MICRO_INSTRUCTION_SIZE - 1) return true;
    return instructions[opcode][ustep].end;
}