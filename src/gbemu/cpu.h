#pragma once

#include "bus.h"
#include "common.h"

/// @brief CPU state
typedef struct Cpu {
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

    bool ime;
    bool ime_scheduled;

    bool halted;

    bool is_jp_interrupt;

    /// @brief Temporary register, cleared before every instruction
    u8 bus_reg;
    /// @brief Temporary register, cleared before every instruction
    u8 jp_lo;
    /// @brief Temporary register, cleared before every instruction
    u8 jp_hi;

    u8 opcode;
    u8 ucode_step;
    u8 prefix_opcode;
} Cpu;

u16 cpu_af(const Cpu *cpu);
u16 cpu_bc(const Cpu *cpu);
u16 cpu_de(const Cpu *cpu);
u16 cpu_hl(const Cpu *cpu);

/// @brief Reset the CPU to zero state
/// @param cpu non-NULL
void cpu_reset(Cpu *cpu);

/// @brief Advance the CPU one cycle (micro-instruction)
/// @param cpu non-NULL
/// @param bus non-NULL
void cpu_cycle(Cpu *cpu, Bus *bus);

/// @brief Print the current CPU state to stdout
/// @param cpu non-NULL
/// @param bus non-NULL
void cpu_print_trace(const Cpu *cpu, const Bus *bus);
