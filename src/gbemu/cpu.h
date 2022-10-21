#pragma once

#include "bus.h"
#include "common.h"

typedef struct {
    u8 a;
    u8 f;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc;
    u16 sp;

    bool halted;
    size_t cycle;

    u8 bus_reg;
    u8 tmp_lo;
    u8 tmp_hi;

    u8 opcode;
    u8 ucode_step;
} Cpu;

u16 to_u16(u8 hi, u8 lo);
u16 cpu_af(Cpu *cpu);
u16 cpu_bc(Cpu *cpu);
u16 cpu_de(Cpu *cpu);
u16 cpu_hl(Cpu *cpu);

void cpu_cycle(Cpu *cpu, Bus *bus);

void cpu_print_info(Cpu *cpu);