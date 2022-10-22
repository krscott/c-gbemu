#!/usr/bin/env python

from sys import stderr
import sys
from typing import Any, Optional


dest_file = open("src/gbemu/instructions.c", "w")


def microinst_str(uinst: dict[str, Any]) -> str:
    if not uinst:
        return "0"
    return ", ".join(f".{k} = {v}" for k, v in uinst.items())


def op_str(op: int, instructions: list[dict[str, Any]]) -> str:
    sep = "},\n              {"
    s = sep.join(microinst_str(uinst) for uinst in instructions)
    return f"    [0x{op:02X}] = {{{{{s}}}}},"


R8_TARGETS = [
    "B", "C", "D", "E", "H", "L", "BUS", "A"
]

ADC_OPS = [
    "ADD", "ADC", "SUB", "SBC", "AND", "XOR", "OR", "CP"
]

M8_WRITE_TARGETS = [
    "WRITE_BC", "WRITE_DE", "WRITE_HL_INC", "WRITE_HL_DEC"
]

M8_READ_TARGETS = [
    "READ_BC", "READ_DE", "READ_HL_INC", "READ_HL_DEC"
]

R16_TARGETS = [
    ["B", "C"], ["D", "E"], ["H", "L"], ["SP_HI", "SP_LO"]
]

PP_TARGETS = [
    ["B", "C"], ["D", "E"], ["H", "L"], ["A", "F"]
]

COND = ["COND_NZ", "COND_Z", "COND_NC", "COND_C"]


def op_instr(op: int) -> Optional[list[dict[str, Any]]]:
    # Top quarter
    if op <= 0x3F:
        # NOP
        if op == 0x00:
            return [{}]

        # LD (u16),SP
        if op == 0x08:
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "JP_HI",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "BUS",
                    "rhs": "SP_LO", "io": "WRITE_JP_INC"},
                {"uop": "LD_R8_R8", "lhs": "BUS",
                    "rhs": "SP_HI", "io": "WRITE_JP_INC"},
            ]

        # JR i8
        if op == 0x18:
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "JP_REL"}
            ]

        # JR cond,i8
        if op & 7 == 0:
            cond = COND[(op >> 4) & 3]

            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC", "cond": cond},
                {"uop": "JP_REL"}
            ]

        # LD r16,u16
        if op & 0xF == 1:
            [lhs_hi, lhs_lo] = R16_TARGETS[(op >> 4) & 3]

            return [
                {},
                {"uop": "LD_R8_R8", "lhs": lhs_lo,
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": lhs_hi,
                    "rhs": "BUS", "io": "READ_PC_INC"},
            ]

        # LD m8,A
        if op & 0xF == 2:
            io = M8_WRITE_TARGETS[op & 3]
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "BUS", "rhs": "A", "io": io}
            ]

        # INC r16
        if op & 0xF == 3:
            [lhs, rhs] = R16_TARGETS[(op >> 4) & 3]

            return [
                {"uop": "INC16", "lhs": lhs, "rhs": rhs},
                {},
            ]

        # ADD r16,r16
        if op & 0xF == 9:
            [rhs_hi, rhs_lo] = R16_TARGETS[(op >> 4) & 3]

            return [
                {"uop": "ADD16_LO", "lhs": "L", "rhs": rhs_lo},
                {"uop": "ADD16_HI", "lhs": "H", "rhs": rhs_hi},
            ]

        # LD A,m8
        if op & 0xF == 0xA:
            io = M8_READ_TARGETS[op & 3]
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "A", "rhs": "BUS", "io": io}
            ]

        # DEC r16
        if op & 0xF == 0xB:
            [lhs, rhs] = R16_TARGETS[(op >> 4) & 3]

            return [
                {"uop": "DEC16", "lhs": lhs, "rhs": rhs},
                {},
            ]

        # INC/DEC d8
        if op & 7 in (4, 5):
            uop = "INC" if op & 7 == 4 else "DEC"
            lhs = R8_TARGETS[(op >> 3) & 7]

            if lhs == "BUS":
                return [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "BUS", "io": "READ_HL"},
                    {"uop": uop, "lhs": "BUS", "io": "WRITE_HL"}
                ]

            else:
                return [{"uop": uop, "lhs": lhs}]

        # LD d8,u8
        if op & 7 == 6:
            lhs = R8_TARGETS[(op >> 3) & 7]

            uinst = {
                "uop": "LD_R8_R8", "lhs": lhs, "rhs": "BUS", "io": "READ_PC_INC"
            }

            if lhs == "BUS":
                return [
                    {},
                    uinst,
                    {"uop": "LD_R8_R8", "lhs": "BUS", "io": "WRITE_HL"}
                ]

            return [{}, uinst]

        if op == 0x07:
            return [{"uop": "RLCA"}]
        if op == 0x0F:
            return [{"uop": "RRCA"}]
        if op == 0x17:
            return [{"uop": "RLA"}]
        if op == 0x1F:
            return [{"uop": "RRA"}]
        if op == 0x27:
            return [{"uop": "DAA"}]
        if op == 0x2F:
            return [{"uop": "CPL"}]
        if op == 0x37:
            return [{"uop": "SCF"}]
        if op == 0x3F:
            return [{"uop": "CCF"}]

    # HALT
    if op == 0x76:
        return [{"uop": "HALT"}]

    # LD d8,d8
    if op >= 0x40 and op <= 0x7F:
        lhs = R8_TARGETS[(op >> 3) & 7]
        rhs = R8_TARGETS[op & 7]
        uinst = {"uop": "LD_R8_R8", "lhs": lhs, "rhs": rhs}

        if lhs == "BUS":
            uinst["io"] = "WRITE_HL"
        if rhs == "BUS":
            uinst["io"] = "READ_HL"

        if lhs == "BUS" or rhs == "BUS":
            return [{}, uinst]

        return [uinst]

    # ALU d8,d8
    if op >= 0x80 and op <= 0xBF:
        uop = ADC_OPS[(op >> 3) & 7]
        rhs = R8_TARGETS[op & 7]
        uinst = {"uop": uop, "lhs": "A", "rhs": rhs}

        if rhs == "BUS":
            uinst["io"] = "READ_HL"
            return [{}, uinst]

        return [uinst]

    # Bottom quarter
    if op >= 0xC0 and op <= 0xFF:
        # RET cond
        if op in (0xC0, 0xC8, 0xD0, 0xD8):
            cond = COND[(op >> 4) & 3]
            return [
                {},
                {"cond": cond},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_SP_INC"},
                {"uop": "LD_R8_R8", "lhs": "JP_HI",
                    "rhs": "BUS", "io": "READ_SP_INC"},
                {"uop": "JP"}
            ]

        # LD (FF00+u8),A
        if op == 0xE0:
            return [
                {"uop": "LD_R8_R8", "lhs": "JP_HI", "rhs": "IM_FF"},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "BUS", "rhs": "A", "io": "WRITE_JP_INC"},
            ]

        # ADD SP,i8
        if op == 0xE8:
            return [
                {},
                # Timing might be wrong, but it shouldn't matter
                {"uop": "ADD16_SP_I8", "lhs": "BUS", "io": "READ_PC_INC"},
                {},
                {},
            ]

        # LD A,(FF00+u8)
        if op == 0xF0:
            return [
                {"uop": "LD_R8_R8", "lhs": "JP_HI", "rhs": "IM_FF"},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "A", "rhs": "BUS", "io": "READ_JP"},
            ]

        # LD HL,SP+i8
        if op == 0xF8:
            return [
                {},
                # Timing might be wrong, but it shouldn't matter
                {"uop": "ADD16_HL_SP_PLUS_I8", "lhs": "BUS", "io": "READ_PC_INC"},
                {},
            ]

        # RET/RETI
        # Cannot combine with `RET cond` because these have 1 less cycle
        if op in (0xC9, 0xD9):
            return [
                {} if op == 0xC9 else {"uop": "ENABLE_INTERRUPTS"},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_SP_INC"},
                {"uop": "LD_R8_R8", "lhs": "JP_HI",
                    "rhs": "BUS", "io": "READ_SP_INC"},
                {"uop": "JP"}
            ]

        # JP HL
        if op == 0xE9:
            return [{"uop": "JP_HL"}]

        # POP r16
        if op & 0xF == 1:
            [hi, lo] = PP_TARGETS[(op >> 4) & 3]
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": lo, "rhs": "BUS", "io": "READ_SP_INC"},
                {"uop": "LD_R8_R8", "lhs": hi, "rhs": "BUS", "io": "READ_SP_INC"},
            ]

        # LD SP,HL
        if op == 0xF9:
            return [
                {"uop": "LD_R8_R8", "lhs": "SP_LO", "rhs": "L"},
                {"uop": "LD_R8_R8", "lhs": "SP_HI", "rhs": "H"},
            ]

        # JP cond,u16 / JP u16
        if op in (0xC2, 0xC3, 0xCA, 0xD2, 0xDA):
            cond = "COND_ALWAYS" if op == 0xC3 else COND[(op >> 4) & 3]
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "JP_HI",
                    "rhs": "BUS", "io": "READ_PC_INC", "cond": cond},
                {"uop": "JP"},
            ]

        # LD (FF00+C),A
        if op == 0xE2:
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "BUS", "rhs": "A", "io": "WRITE_FF00_C"},
            ]

        # LD (u16),A
        if op == 0xEA:
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "JP_HI",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "BUS", "rhs": "A", "io": "WRITE_JP_INC"},
            ]

        # LD (FF00+C),A
        if op == 0xF2:
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "A", "rhs": "BUS", "io": "READ_FF00_C"},
            ]

        # LD A,(u16)
        if op == 0xFA:
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "JP_HI",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "A", "rhs": "BUS", "io": "READ_JP"},
            ]

        # Prefix CB
        if op == 0xCB:
            return [
                {},
                {"uop": "PREFIX_OP", "io": "READ_PC_INC"}
            ]

        # DI
        if op == 0xF3:
            return [{"uop": "DISABLE_INTERRUPTS"}]

        # EI
        if op == 0xFB:
            return [{"uop": "ENABLE_INTERRUPTS"}]

        # CALL cond,u16 / CALL u16
        if op in (0xC4, 0xCC, 0xCD, 0xD4, 0xDC):
            cond = "COND_ALWAYS" if op == 0xCD else COND[(op >> 4) & 3]
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "JP_LO",
                    "rhs": "BUS", "io": "READ_PC_INC"},
                {"uop": "LD_R8_R8", "lhs": "JP_HI",
                    "rhs": "BUS", "io": "READ_PC_INC", "cond": cond},
                # Timing of these writes might be wrong, but it shouldn't matter
                {"uop": "LD_R8_R8", "lhs": "BUS",
                    "rhs": "PC_HI", "io": "WRITE_SP_DEC"},
                {"uop": "LD_R8_R8", "lhs": "BUS",
                    "rhs": "PC_LO", "io": "WRITE_SP_DEC"},
                {"uop": "JP"},
            ]

        # PUSH r16
        if op & 0xF == 5:
            [hi, lo] = PP_TARGETS[(op >> 4) & 3]
            return [
                {},
                {"uop": "LD_R8_R8", "lhs": "BUS", "rhs": lo, "io": "WRITE_SP_DEC"},
                {"uop": "LD_R8_R8", "lhs": "BUS", "rhs": hi, "io": "WRITE_SP_DEC"},
            ]

        # ALU d8,u8
        if op & 7 == 6:
            uop = ADC_OPS[(op >> 3) & 7]

            return [{}, {"uop": uop, "lhs": "A", "rhs": "BUS", "io": "READ_PC_INC"}]

        if op & 7 == 7:
            return [
                {},
                # Timing of these writes might be wrong, but it shouldn't matter
                {"uop": "LD_R8_R8", "lhs": "BUS",
                    "rhs": "PC_HI", "io": "WRITE_SP_DEC"},
                {"uop": "LD_R8_R8", "lhs": "BUS",
                    "rhs": "PC_LO", "io": "WRITE_SP_DEC"},
                {"uop": "RST"}
            ]

    # Undefined opcodes
    if op in (0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD):
        return [{"undefined": "true"}]

    return None


print(f"""
/*
 * NOTE: This file is automatically generated by {sys.argv[0]}
 */

#include "instructions.h"

// clang-format off
const Instruction instructions[0x100] = {{""", file=dest_file)

for op in range(0x100):
    instr = op_instr(op)

    if instr and len(instr) > 0:
        instr[-1]["end"] = "true"
    else:
        print(f"Unhandled op: 0x{op:02X}", file=stderr)
        instr = [{"undefined": "true"}]

    print(op_str(op, instr), file=dest_file)


print("""};
// clang-format on

const MicroInstr *instructions_get_uinst(u8 opcode, u8 ustep) {
    assert(ustep < MICRO_INSTRUCTION_SIZE);
    return &instructions[opcode][ustep];
}

bool instructions_is_last_ustep(u8 opcode, u8 ustep) {
    if (ustep >= MICRO_INSTRUCTION_SIZE - 1) return true;
    return instructions[opcode][ustep].end;
}""", file=dest_file)
