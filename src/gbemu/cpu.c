#include "cpu.h"

#include "instructions.h"

u16 cpu_tmp16(Cpu *cpu) {
    return (((u16)cpu->tmp_hi) << 8) | ((u16)cpu->tmp_lo);
}

void _cpu_inc_counters(Cpu *cpu) {
    ++cpu->cycle;
    ++cpu->ucode_step;
}

void cpu_cycle(Cpu *cpu, Bus *bus) {
    assert(cpu);

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

    u8 io_in = 0;

    switch (uinst->io) {
        case IO_NONE:
            break;
        case FETCH_PC:
            io_in = bus_read(bus, cpu->pc++);
            break;
        default:
            panicf("Unhandled input case: %d", uinst->io);
    }

    switch (uinst->dst) {
        case TARGET_NONE:
            break;
        case A:
            cpu->a = io_in;
            break;
        case B:
            cpu->b = io_in;
            break;
        case C:
            cpu->c = io_in;
            break;
        case D:
            cpu->d = io_in;
            break;
        case E:
            cpu->e = io_in;
            break;
        case H:
            cpu->h = io_in;
            break;
        case L:
            cpu->l = io_in;
            break;
        case TMP_LO:
            cpu->tmp_lo = io_in;
            break;
        case TMP_HI:
            cpu->tmp_hi = io_in;
            break;
        default:
            panicf("Unhandled destination case: %d", uinst->dst);
    }

    switch (uinst->uop) {
        case UOP_NONE:
            break;
        case STORE_PC:
            cpu->pc = cpu_tmp16(cpu);
            break;
        default:
            panicf("Unhandled micro-op case: %d", uinst->dst);
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