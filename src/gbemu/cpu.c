#include "cpu.h"

#include "instructions.h"

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10

u16 cpu_tmp16(Cpu *cpu) {
    return (((u16)cpu->tmp_hi) << 8) | ((u16)cpu->tmp_lo);
}

void _cpu_inc_counters(Cpu *cpu) {
    if (instructions_is_last_ustep(cpu->opcode, cpu->ucode_step)) {
        cpu->ucode_step = 0;
    } else {
        ++cpu->ucode_step;
    }

    ++cpu->cycle;
}

void _cpu_set(Cpu *cpu, Target target, u8 value) {
    switch (target) {
        case TARGET_NONE:
            break;
        case A:
            cpu->a = value;
            break;
        case B:
            cpu->b = value;
            break;
        case C:
            cpu->c = value;
            break;
        case D:
            cpu->d = value;
            break;
        case E:
            cpu->e = value;
            break;
        case H:
            cpu->h = value;
            break;
        case L:
            cpu->l = value;
            break;
        case TMP_LO:
            cpu->tmp_lo = value;
            break;
        case TMP_HI:
            cpu->tmp_hi = value;
            break;
        default:
            panicf("Unhandled load-target case: %d", target);
    }
}

u8 _cpu_get(Cpu *cpu, Target target) {
    switch (target) {
        case TARGET_NONE:
            return 0;
        case A:
            return cpu->a;
        case B:
            return cpu->b;
        case C:
            return cpu->c;
        case D:
            return cpu->d;
        case E:
            return cpu->e;
        case H:
            return cpu->h;
        case L:
            return cpu->l;
        case TMP_LO:
            return cpu->tmp_lo;
        case TMP_HI:
            return cpu->tmp_hi;
        default:
            panicf("Unhandled load-target case: %d", target);
    }
}

/// @brief Get zero flag
/// @param value
/// @return 0x80 if value == 0, otherwise 0
u8 chk_z(u8 value) { return value == 0 ? FZ : 0; }

void cpu_cycle(Cpu *cpu, Bus *bus) {
    assert(cpu);
    if (cpu->halted) return;

    if (cpu->ucode_step == 0) {
        cpu->opcode = bus_read(bus, cpu->pc++);
        cpu->tmp_lo = 0;
        cpu->tmp_hi = 0;
    }

    const MicroInstr *uinst =
        instructions_get_uinst(cpu->opcode, cpu->ucode_step);

    // Make sure we aren't doing multiple bus interactions on the same step
    if (cpu->ucode_step == 0 && uinst->io != IO_NONE) {
        panicf("Illegal IO on step 0 in opcode $%02X", cpu->opcode);
    }

    u8 ld_val = 0;
    u8 alu0_val = 0;
    u8 alu1_val = 0;
    u8 st_val = 0;

    switch (uinst->io) {
        case IO_NONE:
            break;
        case FETCH_PC:
            ld_val = bus_read(bus, cpu->pc++);
            break;
        default:
            panicf("Unhandled input case: %d", uinst->io);
    }

    _cpu_set(cpu, uinst->ld, ld_val);

    alu0_val = _cpu_get(cpu, uinst->alu0);
    alu1_val = _cpu_get(cpu, uinst->alu1);

    // By default, ld_val should pass-through to st_val
    st_val = ld_val;

    switch (uinst->uop) {
        case UOP_NONE:
            break;
        case STORE_PC:
            cpu->pc = cpu_tmp16(cpu);
            break;
        case XOR:
            st_val = alu0_val ^ alu1_val;
            cpu->f = chk_z(st_val);
            break;
        default:
            panicf("Unhandled micro-op case: %d", uinst->uop);
    }

    _cpu_set(cpu, uinst->st, st_val);

    if (uinst->halt) {
        cpu->halted = true;
    }

    _cpu_inc_counters(cpu);
}

void cpu_print_info(Cpu *cpu) {
    printf("CPU Cycle: %lld\n", cpu->cycle);
    printf("  A: %02X  F: %02X\n", cpu->a, cpu->f);
    printf("  B: %02X  C: %02X\n", cpu->b, cpu->c);
    printf("  D: %02X  E: %02X\n", cpu->d, cpu->e);
    printf("  H: %02X  L: %02X\n", cpu->h, cpu->l);
    printf("  SP: %04X\n", cpu->sp);
    printf("  PC: %04X\n", cpu->pc);
    printf("  op: %02X  step: %d\n", cpu->opcode, cpu->ucode_step);
}