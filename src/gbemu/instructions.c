
/*
 * NOTE: This file is automatically generated by gen_instr.py
 */

#include "instructions.h"

// clang-format off
const Instruction instructions[0x100] = {
    [0x00] = {"NOP", 1,
              {{.end = true}}},
    [0x01] = {"LD BC,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = C, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = B, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x02] = {"LD (BC),A", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_BC, .end = true}}},
    [0x03] = {"INC BC", 1,
              {{.uop = INC16, .lhs = B, .rhs = C},
               {.end = true}}},
    [0x04] = {"INC B", 1,
              {{.uop = INC, .lhs = B, .end = true}}},
    [0x05] = {"DEC B", 1,
              {{.uop = DEC, .lhs = B, .end = true}}},
    [0x06] = {"LD B,u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = B, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x07] = {"RLCA", 1,
              {{.uop = RLCA, .end = true}}},
    [0x08] = {"LD (u16),SP", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = SP_LO, .io = WRITE_JP_INC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = SP_HI, .io = WRITE_JP_INC, .end = true}}},
    [0x09] = {"INC BC", 1,
              {{.uop = ADD16_LO, .lhs = L, .rhs = C},
               {.uop = ADD16_HI, .lhs = H, .rhs = B, .end = true}}},
    [0x0A] = {"LD A,(BC)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_BC, .end = true}}},
    [0x0B] = {"DEC BC", 1,
              {{.uop = DEC16, .lhs = B, .rhs = C},
               {.end = true}}},
    [0x0C] = {"INC C", 1,
              {{.uop = INC, .lhs = C, .end = true}}},
    [0x0D] = {"DEC C", 1,
              {{.uop = DEC, .lhs = C, .end = true}}},
    [0x0E] = {"LD C,u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = C, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x0F] = {"RRCA", 1,
              {{.uop = RRCA, .end = true}}},
    [0x10] = {"STOP", 2,
              {{0},
               {.uop = HALT, .io = READ_PC_INC, .end = true}}},
    [0x11] = {"LD DE,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = E, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = D, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x12] = {"LD (DE),A", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_DE, .end = true}}},
    [0x13] = {"INC DE", 1,
              {{.uop = INC16, .lhs = D, .rhs = E},
               {.end = true}}},
    [0x14] = {"INC D", 1,
              {{.uop = INC, .lhs = D, .end = true}}},
    [0x15] = {"DEC D", 1,
              {{.uop = DEC, .lhs = D, .end = true}}},
    [0x16] = {"LD D,u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = D, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x17] = {"RLA", 1,
              {{.uop = RLA, .end = true}}},
    [0x18] = {"JR i8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = JP_REL, .end = true}}},
    [0x19] = {"INC DE", 1,
              {{.uop = ADD16_LO, .lhs = L, .rhs = E},
               {.uop = ADD16_HI, .lhs = H, .rhs = D, .end = true}}},
    [0x1A] = {"LD A,(DE)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_DE, .end = true}}},
    [0x1B] = {"DEC DE", 1,
              {{.uop = DEC16, .lhs = D, .rhs = E},
               {.end = true}}},
    [0x1C] = {"INC E", 1,
              {{.uop = INC, .lhs = E, .end = true}}},
    [0x1D] = {"DEC E", 1,
              {{.uop = DEC, .lhs = E, .end = true}}},
    [0x1E] = {"LD E,u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = E, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x1F] = {"RRA", 1,
              {{.uop = RRA, .end = true}}},
    [0x20] = {"JR NZ,i8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC, .cond = COND_NZ},
               {.uop = JP_REL, .end = true}}},
    [0x21] = {"LD HL,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = L, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = H, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x22] = {"LD (HL+),A", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_HL_INC, .end = true}}},
    [0x23] = {"INC HL", 1,
              {{.uop = INC16, .lhs = H, .rhs = L},
               {.end = true}}},
    [0x24] = {"INC H", 1,
              {{.uop = INC, .lhs = H, .end = true}}},
    [0x25] = {"DEC H", 1,
              {{.uop = DEC, .lhs = H, .end = true}}},
    [0x26] = {"LD H,u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = H, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x27] = {"DAA", 1,
              {{.uop = DAA, .end = true}}},
    [0x28] = {"JR Z,i8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC, .cond = COND_Z},
               {.uop = JP_REL, .end = true}}},
    [0x29] = {"INC HL", 1,
              {{.uop = ADD16_LO, .lhs = L, .rhs = L},
               {.uop = ADD16_HI, .lhs = H, .rhs = H, .end = true}}},
    [0x2A] = {"LD A,(HL+)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_HL_INC, .end = true}}},
    [0x2B] = {"DEC HL", 1,
              {{.uop = DEC16, .lhs = H, .rhs = L},
               {.end = true}}},
    [0x2C] = {"INC L", 1,
              {{.uop = INC, .lhs = L, .end = true}}},
    [0x2D] = {"DEC L", 1,
              {{.uop = DEC, .lhs = L, .end = true}}},
    [0x2E] = {"LD L,u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = L, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x2F] = {"CPL", 1,
              {{.uop = CPL, .end = true}}},
    [0x30] = {"JR NC,i8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC, .cond = COND_NC},
               {.uop = JP_REL, .end = true}}},
    [0x31] = {"LD SP,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = SP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = SP_HI, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x32] = {"LD (HL-),A", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_HL_DEC, .end = true}}},
    [0x33] = {"INC SP", 1,
              {{.uop = INC16, .lhs = SP_HI, .rhs = SP_LO},
               {.end = true}}},
    [0x34] = {"INC (HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .io = READ_HL},
               {.uop = INC, .lhs = BUS, .io = WRITE_HL, .end = true}}},
    [0x35] = {"DEC (HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .io = READ_HL},
               {.uop = DEC, .lhs = BUS, .io = WRITE_HL, .end = true}}},
    [0x36] = {"LD (HL),u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = BUS, .io = WRITE_HL, .end = true}}},
    [0x37] = {"SCF", 1,
              {{.uop = SCF, .end = true}}},
    [0x38] = {"JR C,i8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC, .cond = COND_C},
               {.uop = JP_REL, .end = true}}},
    [0x39] = {"INC SP", 1,
              {{.uop = ADD16_LO, .lhs = L, .rhs = SP_LO},
               {.uop = ADD16_HI, .lhs = H, .rhs = SP_HI, .end = true}}},
    [0x3A] = {"LD A,(HL-)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_HL_DEC, .end = true}}},
    [0x3B] = {"DEC SP", 1,
              {{.uop = DEC16, .lhs = SP_HI, .rhs = SP_LO},
               {.end = true}}},
    [0x3C] = {"INC A", 1,
              {{.uop = INC, .lhs = A, .end = true}}},
    [0x3D] = {"DEC A", 1,
              {{.uop = DEC, .lhs = A, .end = true}}},
    [0x3E] = {"LD A,u8", 2,
              {{0},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0x3F] = {"CCF", 1,
              {{.uop = CCF, .end = true}}},
    [0x40] = {"LD B,B", 1,
              {{.uop = LD_R8_R8, .lhs = B, .rhs = B, .end = true}}},
    [0x41] = {"LD B,C", 1,
              {{.uop = LD_R8_R8, .lhs = B, .rhs = C, .end = true}}},
    [0x42] = {"LD B,D", 1,
              {{.uop = LD_R8_R8, .lhs = B, .rhs = D, .end = true}}},
    [0x43] = {"LD B,E", 1,
              {{.uop = LD_R8_R8, .lhs = B, .rhs = E, .end = true}}},
    [0x44] = {"LD B,H", 1,
              {{.uop = LD_R8_R8, .lhs = B, .rhs = H, .end = true}}},
    [0x45] = {"LD B,L", 1,
              {{.uop = LD_R8_R8, .lhs = B, .rhs = L, .end = true}}},
    [0x46] = {"LD B,(HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = B, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x47] = {"LD B,A", 1,
              {{.uop = LD_R8_R8, .lhs = B, .rhs = A, .end = true}}},
    [0x48] = {"LD C,B", 1,
              {{.uop = LD_R8_R8, .lhs = C, .rhs = B, .end = true}}},
    [0x49] = {"LD C,C", 1,
              {{.uop = LD_R8_R8, .lhs = C, .rhs = C, .end = true}}},
    [0x4A] = {"LD C,D", 1,
              {{.uop = LD_R8_R8, .lhs = C, .rhs = D, .end = true}}},
    [0x4B] = {"LD C,E", 1,
              {{.uop = LD_R8_R8, .lhs = C, .rhs = E, .end = true}}},
    [0x4C] = {"LD C,H", 1,
              {{.uop = LD_R8_R8, .lhs = C, .rhs = H, .end = true}}},
    [0x4D] = {"LD C,L", 1,
              {{.uop = LD_R8_R8, .lhs = C, .rhs = L, .end = true}}},
    [0x4E] = {"LD C,(HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = C, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x4F] = {"LD C,A", 1,
              {{.uop = LD_R8_R8, .lhs = C, .rhs = A, .end = true}}},
    [0x50] = {"LD D,B", 1,
              {{.uop = LD_R8_R8, .lhs = D, .rhs = B, .end = true}}},
    [0x51] = {"LD D,C", 1,
              {{.uop = LD_R8_R8, .lhs = D, .rhs = C, .end = true}}},
    [0x52] = {"LD D,D", 1,
              {{.uop = LD_R8_R8, .lhs = D, .rhs = D, .end = true}}},
    [0x53] = {"LD D,E", 1,
              {{.uop = LD_R8_R8, .lhs = D, .rhs = E, .end = true}}},
    [0x54] = {"LD D,H", 1,
              {{.uop = LD_R8_R8, .lhs = D, .rhs = H, .end = true}}},
    [0x55] = {"LD D,L", 1,
              {{.uop = LD_R8_R8, .lhs = D, .rhs = L, .end = true}}},
    [0x56] = {"LD D,(HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = D, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x57] = {"LD D,A", 1,
              {{.uop = LD_R8_R8, .lhs = D, .rhs = A, .end = true}}},
    [0x58] = {"LD E,B", 1,
              {{.uop = LD_R8_R8, .lhs = E, .rhs = B, .end = true}}},
    [0x59] = {"LD E,C", 1,
              {{.uop = LD_R8_R8, .lhs = E, .rhs = C, .end = true}}},
    [0x5A] = {"LD E,D", 1,
              {{.uop = LD_R8_R8, .lhs = E, .rhs = D, .end = true}}},
    [0x5B] = {"LD E,E", 1,
              {{.uop = LD_R8_R8, .lhs = E, .rhs = E, .end = true}}},
    [0x5C] = {"LD E,H", 1,
              {{.uop = LD_R8_R8, .lhs = E, .rhs = H, .end = true}}},
    [0x5D] = {"LD E,L", 1,
              {{.uop = LD_R8_R8, .lhs = E, .rhs = L, .end = true}}},
    [0x5E] = {"LD E,(HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = E, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x5F] = {"LD E,A", 1,
              {{.uop = LD_R8_R8, .lhs = E, .rhs = A, .end = true}}},
    [0x60] = {"LD H,B", 1,
              {{.uop = LD_R8_R8, .lhs = H, .rhs = B, .end = true}}},
    [0x61] = {"LD H,C", 1,
              {{.uop = LD_R8_R8, .lhs = H, .rhs = C, .end = true}}},
    [0x62] = {"LD H,D", 1,
              {{.uop = LD_R8_R8, .lhs = H, .rhs = D, .end = true}}},
    [0x63] = {"LD H,E", 1,
              {{.uop = LD_R8_R8, .lhs = H, .rhs = E, .end = true}}},
    [0x64] = {"LD H,H", 1,
              {{.uop = LD_R8_R8, .lhs = H, .rhs = H, .end = true}}},
    [0x65] = {"LD H,L", 1,
              {{.uop = LD_R8_R8, .lhs = H, .rhs = L, .end = true}}},
    [0x66] = {"LD H,(HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = H, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x67] = {"LD H,A", 1,
              {{.uop = LD_R8_R8, .lhs = H, .rhs = A, .end = true}}},
    [0x68] = {"LD L,B", 1,
              {{.uop = LD_R8_R8, .lhs = L, .rhs = B, .end = true}}},
    [0x69] = {"LD L,C", 1,
              {{.uop = LD_R8_R8, .lhs = L, .rhs = C, .end = true}}},
    [0x6A] = {"LD L,D", 1,
              {{.uop = LD_R8_R8, .lhs = L, .rhs = D, .end = true}}},
    [0x6B] = {"LD L,E", 1,
              {{.uop = LD_R8_R8, .lhs = L, .rhs = E, .end = true}}},
    [0x6C] = {"LD L,H", 1,
              {{.uop = LD_R8_R8, .lhs = L, .rhs = H, .end = true}}},
    [0x6D] = {"LD L,L", 1,
              {{.uop = LD_R8_R8, .lhs = L, .rhs = L, .end = true}}},
    [0x6E] = {"LD L,(HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = L, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x6F] = {"LD L,A", 1,
              {{.uop = LD_R8_R8, .lhs = L, .rhs = A, .end = true}}},
    [0x70] = {"LD (HL),B", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = B, .io = WRITE_HL, .end = true}}},
    [0x71] = {"LD (HL),C", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = C, .io = WRITE_HL, .end = true}}},
    [0x72] = {"LD (HL),D", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = D, .io = WRITE_HL, .end = true}}},
    [0x73] = {"LD (HL),E", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = E, .io = WRITE_HL, .end = true}}},
    [0x74] = {"LD (HL),H", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = H, .io = WRITE_HL, .end = true}}},
    [0x75] = {"LD (HL),L", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = L, .io = WRITE_HL, .end = true}}},
    [0x76] = {"HALT", 1,
              {{.uop = HALT, .end = true}}},
    [0x77] = {"LD (HL),A", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_HL, .end = true}}},
    [0x78] = {"LD A,B", 1,
              {{.uop = LD_R8_R8, .lhs = A, .rhs = B, .end = true}}},
    [0x79] = {"LD A,C", 1,
              {{.uop = LD_R8_R8, .lhs = A, .rhs = C, .end = true}}},
    [0x7A] = {"LD A,D", 1,
              {{.uop = LD_R8_R8, .lhs = A, .rhs = D, .end = true}}},
    [0x7B] = {"LD A,E", 1,
              {{.uop = LD_R8_R8, .lhs = A, .rhs = E, .end = true}}},
    [0x7C] = {"LD A,H", 1,
              {{.uop = LD_R8_R8, .lhs = A, .rhs = H, .end = true}}},
    [0x7D] = {"LD A,L", 1,
              {{.uop = LD_R8_R8, .lhs = A, .rhs = L, .end = true}}},
    [0x7E] = {"LD A,(HL)", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x7F] = {"LD A,A", 1,
              {{.uop = LD_R8_R8, .lhs = A, .rhs = A, .end = true}}},
    [0x80] = {"ADD A,B", 1,
              {{.uop = ADD, .lhs = A, .rhs = B, .end = true}}},
    [0x81] = {"ADD A,C", 1,
              {{.uop = ADD, .lhs = A, .rhs = C, .end = true}}},
    [0x82] = {"ADD A,D", 1,
              {{.uop = ADD, .lhs = A, .rhs = D, .end = true}}},
    [0x83] = {"ADD A,E", 1,
              {{.uop = ADD, .lhs = A, .rhs = E, .end = true}}},
    [0x84] = {"ADD A,H", 1,
              {{.uop = ADD, .lhs = A, .rhs = H, .end = true}}},
    [0x85] = {"ADD A,L", 1,
              {{.uop = ADD, .lhs = A, .rhs = L, .end = true}}},
    [0x86] = {"ADD A,(HL)", 1,
              {{0},
               {.uop = ADD, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x87] = {"ADD A,A", 1,
              {{.uop = ADD, .lhs = A, .rhs = A, .end = true}}},
    [0x88] = {"ADC A,B", 1,
              {{.uop = ADC, .lhs = A, .rhs = B, .end = true}}},
    [0x89] = {"ADC A,C", 1,
              {{.uop = ADC, .lhs = A, .rhs = C, .end = true}}},
    [0x8A] = {"ADC A,D", 1,
              {{.uop = ADC, .lhs = A, .rhs = D, .end = true}}},
    [0x8B] = {"ADC A,E", 1,
              {{.uop = ADC, .lhs = A, .rhs = E, .end = true}}},
    [0x8C] = {"ADC A,H", 1,
              {{.uop = ADC, .lhs = A, .rhs = H, .end = true}}},
    [0x8D] = {"ADC A,L", 1,
              {{.uop = ADC, .lhs = A, .rhs = L, .end = true}}},
    [0x8E] = {"ADC A,(HL)", 1,
              {{0},
               {.uop = ADC, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x8F] = {"ADC A,A", 1,
              {{.uop = ADC, .lhs = A, .rhs = A, .end = true}}},
    [0x90] = {"SUB A,B", 1,
              {{.uop = SUB, .lhs = A, .rhs = B, .end = true}}},
    [0x91] = {"SUB A,C", 1,
              {{.uop = SUB, .lhs = A, .rhs = C, .end = true}}},
    [0x92] = {"SUB A,D", 1,
              {{.uop = SUB, .lhs = A, .rhs = D, .end = true}}},
    [0x93] = {"SUB A,E", 1,
              {{.uop = SUB, .lhs = A, .rhs = E, .end = true}}},
    [0x94] = {"SUB A,H", 1,
              {{.uop = SUB, .lhs = A, .rhs = H, .end = true}}},
    [0x95] = {"SUB A,L", 1,
              {{.uop = SUB, .lhs = A, .rhs = L, .end = true}}},
    [0x96] = {"SUB A,(HL)", 1,
              {{0},
               {.uop = SUB, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x97] = {"SUB A,A", 1,
              {{.uop = SUB, .lhs = A, .rhs = A, .end = true}}},
    [0x98] = {"SBC A,B", 1,
              {{.uop = SBC, .lhs = A, .rhs = B, .end = true}}},
    [0x99] = {"SBC A,C", 1,
              {{.uop = SBC, .lhs = A, .rhs = C, .end = true}}},
    [0x9A] = {"SBC A,D", 1,
              {{.uop = SBC, .lhs = A, .rhs = D, .end = true}}},
    [0x9B] = {"SBC A,E", 1,
              {{.uop = SBC, .lhs = A, .rhs = E, .end = true}}},
    [0x9C] = {"SBC A,H", 1,
              {{.uop = SBC, .lhs = A, .rhs = H, .end = true}}},
    [0x9D] = {"SBC A,L", 1,
              {{.uop = SBC, .lhs = A, .rhs = L, .end = true}}},
    [0x9E] = {"SBC A,(HL)", 1,
              {{0},
               {.uop = SBC, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0x9F] = {"SBC A,A", 1,
              {{.uop = SBC, .lhs = A, .rhs = A, .end = true}}},
    [0xA0] = {"AND A,B", 1,
              {{.uop = AND, .lhs = A, .rhs = B, .end = true}}},
    [0xA1] = {"AND A,C", 1,
              {{.uop = AND, .lhs = A, .rhs = C, .end = true}}},
    [0xA2] = {"AND A,D", 1,
              {{.uop = AND, .lhs = A, .rhs = D, .end = true}}},
    [0xA3] = {"AND A,E", 1,
              {{.uop = AND, .lhs = A, .rhs = E, .end = true}}},
    [0xA4] = {"AND A,H", 1,
              {{.uop = AND, .lhs = A, .rhs = H, .end = true}}},
    [0xA5] = {"AND A,L", 1,
              {{.uop = AND, .lhs = A, .rhs = L, .end = true}}},
    [0xA6] = {"AND A,(HL)", 1,
              {{0},
               {.uop = AND, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0xA7] = {"AND A,A", 1,
              {{.uop = AND, .lhs = A, .rhs = A, .end = true}}},
    [0xA8] = {"XOR A,B", 1,
              {{.uop = XOR, .lhs = A, .rhs = B, .end = true}}},
    [0xA9] = {"XOR A,C", 1,
              {{.uop = XOR, .lhs = A, .rhs = C, .end = true}}},
    [0xAA] = {"XOR A,D", 1,
              {{.uop = XOR, .lhs = A, .rhs = D, .end = true}}},
    [0xAB] = {"XOR A,E", 1,
              {{.uop = XOR, .lhs = A, .rhs = E, .end = true}}},
    [0xAC] = {"XOR A,H", 1,
              {{.uop = XOR, .lhs = A, .rhs = H, .end = true}}},
    [0xAD] = {"XOR A,L", 1,
              {{.uop = XOR, .lhs = A, .rhs = L, .end = true}}},
    [0xAE] = {"XOR A,(HL)", 1,
              {{0},
               {.uop = XOR, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0xAF] = {"XOR A,A", 1,
              {{.uop = XOR, .lhs = A, .rhs = A, .end = true}}},
    [0xB0] = {"OR A,B", 1,
              {{.uop = OR, .lhs = A, .rhs = B, .end = true}}},
    [0xB1] = {"OR A,C", 1,
              {{.uop = OR, .lhs = A, .rhs = C, .end = true}}},
    [0xB2] = {"OR A,D", 1,
              {{.uop = OR, .lhs = A, .rhs = D, .end = true}}},
    [0xB3] = {"OR A,E", 1,
              {{.uop = OR, .lhs = A, .rhs = E, .end = true}}},
    [0xB4] = {"OR A,H", 1,
              {{.uop = OR, .lhs = A, .rhs = H, .end = true}}},
    [0xB5] = {"OR A,L", 1,
              {{.uop = OR, .lhs = A, .rhs = L, .end = true}}},
    [0xB6] = {"OR A,(HL)", 1,
              {{0},
               {.uop = OR, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0xB7] = {"OR A,A", 1,
              {{.uop = OR, .lhs = A, .rhs = A, .end = true}}},
    [0xB8] = {"CP A,B", 1,
              {{.uop = CP, .lhs = A, .rhs = B, .end = true}}},
    [0xB9] = {"CP A,C", 1,
              {{.uop = CP, .lhs = A, .rhs = C, .end = true}}},
    [0xBA] = {"CP A,D", 1,
              {{.uop = CP, .lhs = A, .rhs = D, .end = true}}},
    [0xBB] = {"CP A,E", 1,
              {{.uop = CP, .lhs = A, .rhs = E, .end = true}}},
    [0xBC] = {"CP A,H", 1,
              {{.uop = CP, .lhs = A, .rhs = H, .end = true}}},
    [0xBD] = {"CP A,L", 1,
              {{.uop = CP, .lhs = A, .rhs = L, .end = true}}},
    [0xBE] = {"CP A,(HL)", 1,
              {{0},
               {.uop = CP, .lhs = A, .rhs = BUS, .io = READ_HL, .end = true}}},
    [0xBF] = {"CP A,A", 1,
              {{.uop = CP, .lhs = A, .rhs = A, .end = true}}},
    [0xC0] = {"RET NZ", 1,
              {{0},
               {.cond = COND_NZ},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
               {.uop = JP, .end = true}}},
    [0xC1] = {"POP BC", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = C, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = B, .rhs = BUS, .io = READ_SP_INC, .end = true}}},
    [0xC2] = {"JP NZ,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_NZ},
               {.uop = JP, .end = true}}},
    [0xC3] = {"JP u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_ALWAYS},
               {.uop = JP, .end = true}}},
    [0xC4] = {"CALL NZ,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_NZ},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = JP, .end = true}}},
    [0xC5] = {"PUSH BC", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = B, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = C, .io = WRITE_SP_DEC, .end = true}}},
    [0xC6] = {"ADD A,u8", 2,
              {{0},
               {.uop = ADD, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xC7] = {"RST 00h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
    [0xC8] = {"RET Z", 1,
              {{0},
               {.cond = COND_Z},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
               {.uop = JP, .end = true}}},
    [0xC9] = {"RET", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
               {.uop = JP, .end = true}}},
    [0xCA] = {"JP NZ,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_NZ},
               {.uop = JP, .end = true}}},
    [0xCB] = {"[CB]", 2,
              {{0},
               {.uop = PREFIX_OP, .io = READ_PC_INC, .end = true}}},
    [0xCC] = {"CALL NZ,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_NZ},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = JP, .end = true}}},
    [0xCD] = {"CALL u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_ALWAYS},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = JP, .end = true}}},
    [0xCE] = {"ADC A,u8", 2,
              {{0},
               {.uop = ADC, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xCF] = {"RST 08h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
    [0xD0] = {"RET NC", 1,
              {{0},
               {.cond = COND_NC},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
               {.uop = JP, .end = true}}},
    [0xD1] = {"POP DE", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = E, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = D, .rhs = BUS, .io = READ_SP_INC, .end = true}}},
    [0xD2] = {"JP Z,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_Z},
               {.uop = JP, .end = true}}},
    [0xD3] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xD4] = {"CALL Z,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_Z},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = JP, .end = true}}},
    [0xD5] = {"PUSH DE", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = D, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = E, .io = WRITE_SP_DEC, .end = true}}},
    [0xD6] = {"SUB A,u8", 2,
              {{0},
               {.uop = SUB, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xD7] = {"RST 10h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
    [0xD8] = {"RET C", 1,
              {{0},
               {.cond = COND_C},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
               {.uop = JP, .end = true}}},
    [0xD9] = {"RETI", 1,
              {{.uop = ENABLE_INTERRUPTS},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_SP_INC},
               {.uop = JP, .end = true}}},
    [0xDA] = {"JP Z,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_Z},
               {.uop = JP, .end = true}}},
    [0xDB] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xDC] = {"CALL Z,u16", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC, .cond = COND_Z},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = JP, .end = true}}},
    [0xDD] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xDE] = {"SBC A,u8", 2,
              {{0},
               {.uop = SBC, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xDF] = {"RST 18h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
    [0xE0] = {"LD (FF00+u8),A", 2,
              {{.uop = LD_R8_R8, .lhs = JP_HI, .rhs = IM_FF},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_JP_INC, .end = true}}},
    [0xE1] = {"POP HL", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = L, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = H, .rhs = BUS, .io = READ_SP_INC, .end = true}}},
    [0xE2] = {"LD (FF00+C),A", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_FF00_C, .end = true}}},
    [0xE3] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xE4] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xE5] = {"PUSH HL", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = H, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = L, .io = WRITE_SP_DEC, .end = true}}},
    [0xE6] = {"AND A,u8", 2,
              {{0},
               {.uop = AND, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xE7] = {"RST 20h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
    [0xE8] = {"ADD SP,i8", 2,
              {{0},
               {.uop = ADD16_SP_I8, .lhs = BUS, .io = READ_PC_INC},
               {0},
               {.end = true}}},
    [0xE9] = {"JP HL", 1,
              {{.uop = JP_HL, .end = true}}},
    [0xEA] = {"LD (u16),A", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_JP_INC, .end = true}}},
    [0xEB] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xEC] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xED] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xEE] = {"XOR A,u8", 2,
              {{0},
               {.uop = XOR, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xEF] = {"RST 28h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
    [0xF0] = {"LD A,(FF00+u8)", 2,
              {{.uop = LD_R8_R8, .lhs = JP_HI, .rhs = IM_FF},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_JP, .end = true}}},
    [0xF1] = {"POP AF", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = F, .rhs = BUS, .io = READ_SP_INC},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_SP_INC, .end = true}}},
    [0xF2] = {"LD (FF00+C),A", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_FF00_C, .end = true}}},
    [0xF3] = {"DI", 1,
              {{.uop = DISABLE_INTERRUPTS, .end = true}}},
    [0xF4] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xF5] = {"PUSH AF", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = A, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = F, .io = WRITE_SP_DEC, .end = true}}},
    [0xF6] = {"OR A,u8", 2,
              {{0},
               {.uop = OR, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xF7] = {"RST 30h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
    [0xF8] = {"LD HL,SP+i8", 2,
              {{0},
               {.uop = ADD16_HL_SP_PLUS_I8, .lhs = BUS, .io = READ_PC_INC},
               {.end = true}}},
    [0xF9] = {"LD SP,HL", 1,
              {{.uop = LD_R8_R8, .lhs = SP_LO, .rhs = L},
               {.uop = LD_R8_R8, .lhs = SP_HI, .rhs = H, .end = true}}},
    [0xFA] = {"LD A,(u16)", 3,
              {{0},
               {.uop = LD_R8_R8, .lhs = JP_LO, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = JP_HI, .rhs = BUS, .io = READ_PC_INC},
               {.uop = LD_R8_R8, .lhs = A, .rhs = BUS, .io = READ_JP, .end = true}}},
    [0xFB] = {"EI", 1,
              {{.uop = ENABLE_INTERRUPTS, .end = true}}},
    [0xFC] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xFD] = {"(undefined)", 1,
              {{.undefined = true, .end = true}}},
    [0xFE] = {"CP A,u8", 2,
              {{0},
               {.uop = CP, .lhs = A, .rhs = BUS, .io = READ_PC_INC, .end = true}}},
    [0xFF] = {"RST 38h", 1,
              {{0},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
               {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
               {.uop = RST, .end = true}}},
};
// clang-format on

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep) {
    assert(ustep < MICRO_INSTRUCTION_SIZE);
    return &instructions[opcode].micro_instructions[ustep];
}

bool instructions_is_last_ustep(u8 opcode, u8 ustep) {
    if (ustep >= MICRO_INSTRUCTION_SIZE - 1) return true;
    return instructions[opcode].micro_instructions[ustep].end;
}

const char* instructions_get_mnemonic(u8 opcode) {
    return instructions[opcode].mnemonic;
}

u8 instructions_get_length(u8 opcode) { return instructions[opcode].length; }
