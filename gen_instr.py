#!/usr/bin/env python

from sys import stderr
import sys
from typing import Any


dest_file = open("src/gbemu/instructions.c", "w")


def microinst_str(uinst: dict[str, Any]) -> str:
    if not uinst:
        return "0"
    return ", ".join(f".{k} = {v}" for k, v in uinst.items())


def inst_length(instruction: list[dict[str, Any]]) -> int:
    len = 1
    for uinst in instruction:
        if uinst.get("io") == "READ_PC_INC":
            len += 1
    return len


def op_str(op: int, label: str, instruction: list[dict[str, Any]]) -> str:
    sep = "},\n               {"
    s = sep.join(microinst_str(uinst) for uinst in instruction)

    length = inst_length(instruction)

    return f"    [0x{op:02X}] = {{\"{label}\", {length},\n              {{{{{s}}}}}}},"


R8_TARGETS = [
    "B", "C", "D", "E", "H", "L", "BUS", "A"
]
R8_TARGETS_STR = [
    "B", "C", "D", "E", "H", "L", "(HL)", "A"
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
M8_TARGETS_STR = ["(BC)", "(DE)", "(HL+)", "(HL-)"]

R16_TARGETS = [
    ["B", "C"], ["D", "E"], ["H", "L"], ["SP_HI", "SP_LO"]
]
R16_TARGET_STR = ["BC", "DE", "HL", "SP"]

PP_TARGETS = [
    ["B", "C"], ["D", "E"], ["H", "L"], ["A", "F"]
]
PP_TARGET_STR = ["BC", "DE", "HL", "AF"]

COND = ["COND_NZ", "COND_Z", "COND_NC", "COND_C"]
COND_STR = ["NZ", "Z", "NC", "C"]


def op_instr(op: int) -> tuple[str, list[dict[str, Any]]]:
    # Top quarter
    if op <= 0x3F:
        # NOP
        if op == 0x00:
            return ("NOP", [{}])

        # LD (u16),SP
        if op == 0x08:
            return (
                "LD (u16),SP",
                [
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
            )

        # STOP
        if op == 0x10:
            return (
                "STOP",
                [
                    {},
                    {"uop": "HALT", "io": "READ_PC_INC"},
                ]
            )

        # JR i8
        if op == 0x18:
            return (
                "JR i8",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "JP_REL"}
                ]
            )

        # JR cond,i8
        if op in (0x20, 0x28, 0x30, 0x38):
            i = (op >> 3) & 3
            cond = COND[i]

            return (
                f"JR {COND_STR[i]},i8",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_PC_INC", "cond": cond},
                    {"uop": "JP_REL"}
                ]
            )

        # LD r16,u16
        if op & 0xF == 1:
            i = (op >> 4) & 3
            [lhs_hi, lhs_lo] = R16_TARGETS[i]

            return (
                f"LD {R16_TARGET_STR[i]},u16",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": lhs_lo,
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": lhs_hi,
                     "rhs": "BUS", "io": "READ_PC_INC"},
                ]
            )

        # LD m8,A
        if op & 0xF == 2:
            i = (op >> 4) & 3
            io = M8_WRITE_TARGETS[i]
            return (
                f"LD {M8_TARGETS_STR[i]},A",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "BUS", "rhs": "A", "io": io}
                ]
            )

        # INC r16
        if op & 0xF == 3:
            i = (op >> 4) & 3
            [lhs, rhs] = R16_TARGETS[i]

            return (
                f"INC {R16_TARGET_STR[i]}",
                [
                    {"uop": "INC16", "lhs": lhs, "rhs": rhs},
                    {},
                ]
            )

        # ADD r16,r16
        if op & 0xF == 9:
            i = (op >> 4) & 3
            [rhs_hi, rhs_lo] = R16_TARGETS[i]

            return (
                f"INC {R16_TARGET_STR[i]}",
                [
                    {"uop": "ADD16_LO", "lhs": "L", "rhs": rhs_lo},
                    {"uop": "ADD16_HI", "lhs": "H", "rhs": rhs_hi},
                ]
            )

        # LD A,m8
        if op & 0xF == 0xA:
            i = (op >> 4) & 3
            io = M8_READ_TARGETS[i]
            return (
                f"LD A,{M8_TARGETS_STR[i]}",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "A", "rhs": "BUS", "io": io}
                ]
            )

        # DEC r16
        if op & 0xF == 0xB:
            i = (op >> 4) & 3
            [lhs, rhs] = R16_TARGETS[i]

            return (
                f"DEC {R16_TARGET_STR[i]}",
                [
                    {"uop": "DEC16", "lhs": lhs, "rhs": rhs},
                    {},
                ]
            )

        # INC/DEC d8
        if op & 7 in (4, 5):
            uop = "INC" if op & 7 == 4 else "DEC"
            i = (op >> 3) & 7
            lhs = R8_TARGETS[i]

            label = f"{uop} {R8_TARGETS_STR[i]}"

            if lhs == "BUS":
                return (
                    label,
                    [
                        {},
                        {"uop": uop, "lhs": lhs, "io": "READ_HL"},
                        {"io": "WRITE_HL"}
                    ]
                )

            else:
                return (
                    label,
                    [{"uop": uop, "lhs": lhs}]
                )

        # LD d8,u8
        if op & 7 == 6:
            i = (op >> 3) & 7
            lhs = R8_TARGETS[i]
            label = f"LD {R8_TARGETS_STR[i]},u8"

            uinst = {
                "uop": "LD_R8_R8", "lhs": lhs, "rhs": "BUS", "io": "READ_PC_INC"
            }

            if lhs == "BUS":
                return (
                    label,
                    [
                        {},
                        uinst,
                        {"io": "WRITE_HL"}
                    ]
                )

            return (
                label,
                [{}, uinst]
            )

        if op == 0x07:
            return ("RLCA", [{"uop": "RLCA"}])
        if op == 0x0F:
            return ("RRCA", [{"uop": "RRCA"}])
        if op == 0x17:
            return ("RLA", [{"uop": "RLA"}])
        if op == 0x1F:
            return ("RRA", [{"uop": "RRA"}])
        if op == 0x27:
            return ("DAA", [{"uop": "DAA"}])
        if op == 0x2F:
            return ("CPL", [{"uop": "CPL"}])
        if op == 0x37:
            return ("SCF", [{"uop": "SCF"}])
        if op == 0x3F:
            return ("CCF", [{"uop": "CCF"}])

    # HALT
    if op == 0x76:
        return ("HALT", [{"uop": "HALT"}])

    # LD d8,d8
    if op >= 0x40 and op <= 0x7F:
        lhs_i = (op >> 3) & 7
        rhs_i = op & 7
        lhs = R8_TARGETS[lhs_i]
        rhs = R8_TARGETS[rhs_i]
        uinst = {"uop": "LD_R8_R8", "lhs": lhs, "rhs": rhs}
        label = f"LD {R8_TARGETS_STR[lhs_i]},{R8_TARGETS_STR[rhs_i]}"

        if lhs == "BUS":
            uinst["io"] = "WRITE_HL"
        if rhs == "BUS":
            uinst["io"] = "READ_HL"

        if lhs == "BUS" or rhs == "BUS":
            return (label, [{}, uinst])

        return (label, [uinst])

    # ALU d8,d8
    if op >= 0x80 and op <= 0xBF:
        uop = ADC_OPS[(op >> 3) & 7]
        i = op & 7
        rhs = R8_TARGETS[i]
        uinst = {"uop": uop, "lhs": "A", "rhs": rhs}
        label = f"{uop} A,{R8_TARGETS_STR[i]}"

        if rhs == "BUS":
            uinst["io"] = "READ_HL"
            return (label, [{}, uinst])

        return (label, [uinst])

    # Bottom quarter
    if op >= 0xC0 and op <= 0xFF:
        # RET cond
        if op in (0xC0, 0xC8, 0xD0, 0xD8):
            i = (op >> 3) & 3
            cond = COND[i]
            return (
                f"RET {COND_STR[i]}",
                [
                    {},
                    {"cond": cond},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_SP_INC"},
                    {"uop": "LD_R8_R8", "lhs": "JP_HI",
                     "rhs": "BUS", "io": "READ_SP_INC"},
                    {"uop": "JP"}
                ]
            )

        # LD (FF00+u8),A
        if op == 0xE0:
            return (
                "LD (FF00+u8),A",
                [
                    {"uop": "LD_R8_R8", "lhs": "JP_HI", "rhs": "IM_FF"},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "BUS",
                        "rhs": "A", "io": "WRITE_JP_INC"},
                ]
            )

        # ADD SP,i8
        if op == 0xE8:
            return (
                "ADD SP,i8",
                [
                    {},
                    # Timing might be wrong, but it shouldn't matter
                    {"uop": "ADD16_SP_I8", "lhs": "BUS", "io": "READ_PC_INC"},
                    {},
                    {},
                ]
            )

        # LD A,(FF00+u8)
        if op == 0xF0:
            return (
                "LD A,(FF00+u8)",
                [
                    {"uop": "LD_R8_R8", "lhs": "JP_HI", "rhs": "IM_FF"},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "A", "rhs": "BUS", "io": "READ_JP"},
                ]
            )

        # LD HL,SP+i8
        if op == 0xF8:
            return (
                "LD HL,SP+i8",
                [
                    {},
                    # Timing might be wrong, but it shouldn't matter
                    {"uop": "ADD16_HL_SP_PLUS_I8",
                        "lhs": "BUS", "io": "READ_PC_INC"},
                    {},
                ]
            )

        # RET/RETI
        # Cannot combine with `RET cond` because these have 1 less cycle
        if op in (0xC9, 0xD9):
            return (
                "RET" if op == 0xC9 else "RETI",
                [
                    {} if op == 0xC9 else {"uop": "ENABLE_INTERRUPTS"},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_SP_INC"},
                    {"uop": "LD_R8_R8", "lhs": "JP_HI",
                     "rhs": "BUS", "io": "READ_SP_INC"},
                    {"uop": "JP"}
                ]
            )

        # JP HL
        if op == 0xE9:
            return ("JP HL", [{"uop": "JP_HL"}])

        # POP r16
        if op & 0xF == 1:
            i = (op >> 4) & 3
            [hi, lo] = PP_TARGETS[i]
            return (
                f"POP {PP_TARGET_STR[i]}",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": lo,
                        "rhs": "BUS", "io": "READ_SP_INC"},
                    {"uop": "LD_R8_R8", "lhs": hi,
                        "rhs": "BUS", "io": "READ_SP_INC"},
                ]
            )

        # LD SP,HL
        if op == 0xF9:
            return (
                "LD SP,HL",
                [
                    {"uop": "LD_R8_R8", "lhs": "SP_LO", "rhs": "L"},
                    {"uop": "LD_R8_R8", "lhs": "SP_HI", "rhs": "H"},
                ]
            )

        # JP cond,u16 / JP u16
        if op in (0xC2, 0xC3, 0xCA, 0xD2, 0xDA):
            i = (op >> 4) & 3
            cond = "COND_ALWAYS" if op == 0xC3 else COND[i]
            cond_str = "" if op == 0xC3 else f"{COND_STR[i]},"
            return (
                f"JP {cond_str}u16",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "JP_HI",
                     "rhs": "BUS", "io": "READ_PC_INC", "cond": cond},
                    {"uop": "JP"},
                ]
            )

        # LD (FF00+C),A
        if op == 0xE2:
            return (
                "LD (FF00+C),A",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "BUS",
                        "rhs": "A", "io": "WRITE_FF00_C"},
                ]
            )

        # LD (u16),A
        if op == 0xEA:
            return (
                "LD (u16),A",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "JP_HI",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "BUS",
                        "rhs": "A", "io": "WRITE_JP_INC"},
                ]
            )

        # LD (FF00+C),A
        if op == 0xF2:
            return (
                "LD (FF00+C),A",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "A",
                        "rhs": "BUS", "io": "READ_FF00_C"},
                ]
            )

        # LD A,(u16)
        if op == 0xFA:
            return (
                "LD A,(u16)",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "JP_LO",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "JP_HI",
                     "rhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "A", "rhs": "BUS", "io": "READ_JP"},
                ]
            )

        # Prefix CB
        if op == 0xCB:
            return (
                "[CB]",
                [
                    {},
                    {"uop": "PREFIX_OP_1", "lhs": "BUS", "io": "READ_PC_INC"},
                    {"uop": "LD_R8_R8", "lhs": "BUS", "io": "READ_HL"},
                    {"uop": "PREFIX_OP_2", "lhs": "BUS", "io": "WRITE_HL"},
                ]
            )

        # DI
        if op == 0xF3:
            return ("DI", [{"uop": "DISABLE_INTERRUPTS"}])

        # EI
        if op == 0xFB:
            return ("EI", [{"uop": "ENABLE_INTERRUPTS"}])

        # CALL cond,u16 / CALL u16
        if op in (0xC4, 0xCC, 0xCD, 0xD4, 0xDC):
            i = (op >> 4) & 3
            cond = "COND_ALWAYS" if op == 0xCD else COND[i]
            cond_str = "" if op == 0xCD else f"{COND_STR[i]},"
            return (
                f"CALL {cond_str}u16",
                [
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
            )

        # PUSH r16
        if op & 0xF == 5:
            i = (op >> 4) & 3
            [hi, lo] = PP_TARGETS[i]

            return (
                f"PUSH {PP_TARGET_STR[i]}",
                [
                    {},
                    {"uop": "LD_R8_R8", "lhs": "BUS",
                        "rhs": hi, "io": "WRITE_SP_DEC"},
                    {"uop": "LD_R8_R8", "lhs": "BUS",
                        "rhs": lo, "io": "WRITE_SP_DEC"},
                ]
            )

        # ALU A,u8
        if op & 7 == 6:
            uop = ADC_OPS[(op >> 3) & 7]

            return (
                f"{uop} A,u8",
                [{}, {"uop": uop, "lhs": "A", "rhs": "BUS", "io": "READ_PC_INC"}]
            )

        if op & 7 == 7:
            return (
                f"RST {(op & 0x38):02X}h",
                [
                    {},
                    # Timing of these writes might be wrong, but it shouldn't matter
                    {"uop": "LD_R8_R8", "lhs": "BUS",
                     "rhs": "PC_HI", "io": "WRITE_SP_DEC"},
                    {"uop": "LD_R8_R8", "lhs": "BUS",
                     "rhs": "PC_LO", "io": "WRITE_SP_DEC"},
                    {"uop": "RST"}
                ]
            )

    # Undefined opcodes
    if op in (0xD3, 0xDB, 0xDD, 0xE3, 0xE4, 0xEB, 0xEC, 0xED, 0xF4, 0xFC, 0xFD):
        return ("(undefined)", [{"undefined": "true"}])

    raise RuntimeError(f"Unhandled opcode 0x{op:02X}")


print(f"""
/*
 * NOTE: This file is automatically generated by {sys.argv[0]}
 */

#include "instructions.h"

// clang-format off
const Instruction instructions[0x100] = {{""", file=dest_file)

for op in range(0x100):
    (mnemonic, instr) = op_instr(op)

    if instr and len(instr) > 0:
        instr[-1]["end"] = "true"
    else:
        print(f"Unhandled op: 0x{op:02X}", file=stderr)
        instr = [{"undefined": "true"}]

    print(op_str(op, mnemonic, instr), file=dest_file)


print("""};

const char *prefix_mnemonics[0x100] = {""", file=dest_file)

for op in range(0x100):
    reg = R8_TARGETS_STR[op & 7]
    bit = (op >> 3) & 7
    shiftop = ["RLC", "RRC", "RL", "RR", "SLA", "SRA", "SWAP", "SRL"][bit]

    if op < 0x40:
        s = f"{shiftop} {reg}"
    elif op < 0x80:
        s = f"BIT {bit},{reg}"
    elif op < 0xC0:
        s = f"RES {bit},{reg}"
    else:
        s = f"SET {bit},{reg}"

    if op & 3 == 0:
        print("    ", file=dest_file, end="")

    print(f"\"{s}\",".ljust(14), file=dest_file, end="")

    if op & 3 == 3:
        print("", file=dest_file)


print("""
};
// clang-format on

const MicroInstr* instructions_get_uinst(u8 opcode, u8 ustep) {
    assert(ustep < MICRO_INSTRUCTION_SIZE);
    return &instructions[opcode].micro_instructions[ustep];
}

const char* instructions_get_mnemonic(u8 opcode) {
    return instructions[opcode].mnemonic;
}

const char* instructions_get_prefix_mnemonic(u8 arg) {
    return prefix_mnemonics[arg];
}

u8 instructions_get_length(u8 opcode) { return instructions[opcode].length; }""", file=dest_file)
