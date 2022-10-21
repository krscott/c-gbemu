#include "cpu.h"

#include "instructions.h"

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10

u16 to_u16(u8 hi, u8 lo) { return (((u16)hi) << 8) | ((u16)lo); }
u16 cpu_af(Cpu *cpu) { return to_u16(cpu->a, cpu->f); }
u16 cpu_bc(Cpu *cpu) { return to_u16(cpu->b, cpu->c); }
u16 cpu_de(Cpu *cpu) { return to_u16(cpu->d, cpu->e); }
u16 cpu_hl(Cpu *cpu) { return to_u16(cpu->h, cpu->l); }

u16 _cpu_tmp16(Cpu *cpu) { return to_u16(cpu->tmp_hi, cpu->tmp_lo); }

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
        case BUS:
            cpu->bus_reg = value;
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

u8 _get_target(Cpu *cpu, Target target) {
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
        case BUS:
            return cpu->bus_reg;
        case TMP_LO:
            return cpu->tmp_lo;
        case TMP_HI:
            return cpu->tmp_hi;
        default:
            panicf("Unhandled load-target case: %d", target);
    }
}

u8 chk_z(u8 res) { return res == 0 ? FZ : 0; }
u8 chk_c(u16 res16) { return res16 > 0xFF ? FC : 0; }
u8 chk_h(u8 lhs, u8 rhs, u8 res) { return (lhs ^ rhs ^ res) & 0x10 ? FH : 0; }

u8 neg(u8 x) { return x == 0 ? 0 : (~x) + 1; }

void alu_add(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    u16 res16 = (u16)lhs + (u16)rhs;
    u8 res = (u8)res16;
    *flags = chk_z(res) | chk_h(lhs, rhs, res) | chk_c(res16);
    *out = res;
}

void alu_sub(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    alu_add(lhs, neg(rhs), out, flags);
    *flags |= FN;
}

void alu_inc(u8 lhs, u8 *out, u8 *flags) {
    u8 new_flags = 0;
    alu_add(lhs, 1, out, &new_flags);
    // Preserve carry flag
    *flags = (*flags & FC) | (new_flags & (~FC));
}

void alu_dec(u8 lhs, u8 *out, u8 *flags) {
    u8 new_flags = 0;
    alu_sub(lhs, 1, out, &new_flags);
    // Preserve carry flag
    *flags = (*flags & FC) | (new_flags & (~FC));
}

void alu_xor(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    *out = lhs ^ rhs;
    *flags = chk_z(*out);
}

void cpu_cycle(Cpu *cpu, Bus *bus) {
    assert(cpu);
    if (cpu->halted) return;

    if (cpu->ucode_step == 0) {
        // Read next opcode
        cpu->opcode = bus_read(bus, cpu->pc++);

        // Reset intra-micro-instruction registers to "uninitialized" state
        cpu->tmp_lo = 0xAA;
        cpu->tmp_hi = 0xAA;
        cpu->bus_reg = 0xAA;
    }

    const MicroInstr *uinst =
        instructions_get_uinst(cpu->opcode, cpu->ucode_step);

    if (uinst->undefined) {
        panicf("Executing undefined instr: $%02X (step %d)", cpu->opcode,
               cpu->ucode_step);
    }

    // Make sure we aren't doing multiple bus interactions on the same step
    assert(cpu->ucode_step != 0 || uinst->io == IO_NONE);

    // Section 1 - Bus Read

    switch (uinst->io) {
        case IO_NONE:
        case WRITE_HL:
            break;
        case READ_PC:
            cpu->bus_reg = bus_read(bus, cpu->pc++);
            break;
        default:
            panicf("Unhandled input case: %d", uinst->io);
    }

    // Section 2 - ALU Op

    u8 alu_lhs = _get_target(cpu, uinst->lhs);
    u8 alu_rhs = _get_target(cpu, uinst->rhs);
    u8 ld_val = 0xAA;

    switch (uinst->uop) {
        case UOP_NONE:
            ld_val = cpu->bus_reg;
            break;
        case LD_BUS_LHS:
            cpu->bus_reg = alu_lhs;
            break;
        case LD_PC_TMP:
            cpu->pc = _cpu_tmp16(cpu);
            break;
        case INC:
            alu_inc(alu_lhs, &ld_val, &cpu->f);
            break;
        case DEC:
            alu_dec(alu_lhs, &ld_val, &cpu->f);
            break;
        case ADD:
            alu_add(alu_lhs, alu_rhs, &ld_val, &cpu->f);
            break;
        case SUB:
            alu_sub(alu_lhs, alu_rhs, &ld_val, &cpu->f);
            break;
        case XOR:
            alu_xor(alu_lhs, alu_rhs, &ld_val, &cpu->f);
            break;
        default:
            panicf("Unhandled micro-op case: %d", uinst->uop);
    }

    _cpu_set(cpu, uinst->ld, ld_val);

    // Section 3 - Bus Write

    switch (uinst->io) {
        case IO_NONE:
        case READ_PC:
            break;
        case WRITE_HL:
            bus_write(bus, cpu_hl(cpu), cpu->bus_reg);
            break;
        default:
            panicf("Unhandled output case: %d", uinst->io);
    }

    // Section 4 - Counters/Cleanup

    _cpu_inc_counters(cpu);

    if (uinst->halt) {
        cpu->halted = true;
    }
}

void cpu_print_info(Cpu *cpu) {
    printf("CPU Cycle: %lld\n", cpu->cycle);
    printf("  A: %02X  F: %02X\n", cpu->a, cpu->f);
    printf("  B: %02X  C: %02X\n", cpu->b, cpu->c);
    printf("  D: %02X  E: %02X\n", cpu->d, cpu->e);
    printf("  H: %02X  L: %02X\n", cpu->h, cpu->l);
    printf("  SP: %04X\n", cpu->sp);
    printf("  PC: %04X\n", cpu->pc);
    printf("  tmp: %04X busreg: %02X\n", to_u16(cpu->tmp_hi, cpu->tmp_lo),
           cpu->bus_reg);
    printf("  op: %02X  step: %d\n", cpu->opcode, cpu->ucode_step);
}