#include "cpu.h"

#include "instructions.h"

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10

u8 low_byte(u16 word) { return (u8)(word & 0xFF); }
u8 high_byte(u16 word) { return (u8)(word >> 8); }
u16 to_u16(u8 hi, u8 lo) { return (((u16)hi) << 8) | ((u16)lo); }
void split_u16(u16 value, u8 *hi, u8 *lo) {
    *hi = high_byte(value);
    *lo = low_byte(value);
}

u16 cpu_af(Cpu *cpu) { return to_u16(cpu->a, cpu->f); }
u16 cpu_bc(Cpu *cpu) { return to_u16(cpu->b, cpu->c); }
u16 cpu_de(Cpu *cpu) { return to_u16(cpu->d, cpu->e); }
u16 cpu_hl(Cpu *cpu) { return to_u16(cpu->h, cpu->l); }
u16 cpu_hl_postinc(Cpu *cpu) {
    u16 x = to_u16(cpu->h, cpu->l);
    split_u16(x + 1, &cpu->h, &cpu->l);
    return x;
}
u16 cpu_hl_postdec(Cpu *cpu) {
    u16 x = to_u16(cpu->h, cpu->l);
    split_u16(x - 1, &cpu->h, &cpu->l);
    return x;
}
u16 cpu_jp_reg(Cpu *cpu) { return to_u16(cpu->jp_hi, cpu->jp_lo); }
u16 cpu_jp_reg_postinc(Cpu *cpu) {
    u16 x = to_u16(cpu->jp_hi, cpu->jp_lo);
    split_u16(x + 1, &cpu->jp_hi, &cpu->jp_lo);
    return x;
}

void cpu_set(Cpu *cpu, Target target, u8 value) {
    switch (target) {
        case TARGET_NONE:
            break;
        case A:
            cpu->a = value;
            break;
        case F:
            cpu->f = value;
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
        case SP_LO:
            cpu->sp = to_u16(high_byte(cpu->sp), value);
            break;
        case SP_HI:
            cpu->sp = to_u16(value, low_byte(cpu->sp));
            break;
        case PC_LO:
            cpu->pc = to_u16(high_byte(cpu->pc), value);
            break;
        case PC_HI:
            cpu->pc = to_u16(value, low_byte(cpu->pc));
            break;
        case JP_LO:
            cpu->jp_lo = value;
            break;
        case JP_HI:
            cpu->jp_hi = value;
            break;
        case IM_FF:
            panic("Cannot write to immediate value");
            break;
        default:
            panicf("Unhandled load-target case: %d", target);
    }
}

u8 cpu_get(Cpu *cpu, Target target) {
    switch (target) {
        case TARGET_NONE:
            return 0xAA;
        case A:
            return cpu->a;
        case F:
            return cpu->f;
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
        case SP_LO:
            return low_byte(cpu->sp);
        case SP_HI:
            return high_byte(cpu->sp);
        case PC_LO:
            return low_byte(cpu->pc);
        case PC_HI:
            return high_byte(cpu->pc);
        case JP_LO:
            return cpu->jp_lo;
        case JP_HI:
            return cpu->jp_hi;
        case IM_FF:
            return 0xFF;
        default:
            panicf("Unhandled load-target case: %d", target);
    }
}

#define chk_z(x) (x == 0 ? FZ : 0)

void alu(u8 lhs, u8 rhs, bool sub, bool use_carry, u8 *out, u8 *flags) {
    u16 carry = (use_carry && *flags & FC) ? 1 : 0;

    if (sub) {
        carry = !carry;
        rhs = ~rhs;
    }

    u16 res16 = (u16)lhs + (u16)rhs + carry;
    u8 res8 = (u8)(res16 & 0xFF);

    u8 res_h = (lhs & 0xF) + (rhs & 0xF) + (u8)carry;

    *out = res8;
    *flags = ((res8 == 0 ? FZ : 0) | (sub ? FN : 0) | (res_h > 0x0F ? FH : 0) |
              (res16 > 0xFF ? FC : 0));
}

void cpu_run_alu(Cpu *cpu, Target lhs, Target rhs, bool sub, bool use_carry) {
    u8 lhs_val = cpu_get(cpu, lhs);
    u8 rhs_val = cpu_get(cpu, rhs);
    alu(lhs_val, rhs_val, sub, use_carry, &lhs_val, &cpu->f);
    cpu_set(cpu, lhs, lhs_val);
}

void alu_u16_plus_i8(u16 lhs16, u8 rhs_lo, u16 *out, u8 *flags) {
    u8 lhs_lo = low_byte(lhs16);
    u8 lhs_hi = high_byte(lhs16);
    // Extend sign bit
    u8 rhs_hi = rhs_lo & 0x80 ? 0xFF : 0;
    u8 res_lo, res_hi;
    alu(lhs_lo, rhs_lo, false, false, &res_lo, flags);
    alu(lhs_hi, rhs_hi, false, true, &res_hi, flags);
    *out = to_u16(res_hi, res_lo);
    *flags &= ~(FZ | FN);
}

void alu_inc(u8 lhs, u8 *out, u8 *flags) {
    *out = (u8)((u16)lhs + 1);
    *flags = chk_z(*out) | (*out & 0xF ? 0 : FH) | (*flags & FC);
}

void alu_dec(u8 lhs, u8 *out, u8 *flags) {
    *out = lhs == 0 ? ~lhs : lhs - 1;
    *flags = chk_z(*out) | FN | (*out & 0xF ? FH : 0) | (*flags & FC);
}

void bitwise_and(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    *out = lhs & rhs;
    *flags = chk_z(*out) | FH;
}

void bitwise_xor(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    *out = lhs ^ rhs;
    *flags = chk_z(*out);
}

void bitwise_or(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    *out = lhs | rhs;
    *flags = chk_z(*out);
}

void bitwise_cp(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    // Same as SUB r8,r8 but result is ignored
    (void)out;
    u8 dummy;
    alu(lhs, rhs, true, false, &dummy, flags);
}

void cpu_run_bitwise_op(Cpu *cpu, Target lhs, Target rhs,
                        void (*bitwise_op)(u8, u8, u8 *, u8 *)) {
    u8 lhs_val = cpu_get(cpu, lhs);
    u8 rhs_val = cpu_get(cpu, rhs);
    bitwise_op(lhs_val, rhs_val, &lhs_val, &cpu->f);
    cpu_set(cpu, lhs, lhs_val);
}

void bit_rlc(u8 *value, u8 *flags) {
    *value = to_u16(*value, *value) >> 7;
    *flags = chk_z(*value) | (*value & 1 ? FC : 0);
}

void bit_rrc(u8 *value, u8 *flags) {
    *value = to_u16(*value, *value) >> 1;
    *flags = chk_z(*value) | (*value & 1 ? FC : 0);
}

void bit_rl(u8 *value, u8 *flags) {
    bool new_carry = *value & 0x80;
    // Move carry flag to bit 7 so that it is shifted into 0
    *value = to_u16(*value, *flags << 3) >> 7;
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

void bit_rr(u8 *value, u8 *flags) {
    bool new_carry = *value & 1;
    // Move carry flag to bit 9 so that it is shifted into 8
    *value = to_u16(*flags >> 4, *value) >> 1;
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

void bit_sla(u8 *value, u8 *flags) {
    bool new_carry = *value & 0x80;
    *value <<= 1;
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

void bit_sra(u8 *value, u8 *flags) {
    bool new_carry = *value & 1;
    // Arithmetic shift: sign bit is preserved
    *value = (*value & 0x80) | (*value >> 1);
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

void bit_srl(u8 *value, u8 *flags) {
    bool new_carry = *value & 1;
    *value = *value >> 1;
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

void bit_swap(u8 *value, u8 *flags) {
    *value = (*value << 4) | (*value >> 4);
    *flags = chk_z(*value);
}

void cpu_run_bitshift_op(Cpu *cpu, Target target,
                         void (*bitshift_op)(u8 *, u8 *)) {
    u8 value = cpu_get(cpu, target);
    bitshift_op(&value, &cpu->f);
    cpu_set(cpu, target, value);
}

void cpu_run_prefix_op(Cpu *cpu, Target target) {
    u8 bitmask = 1 << ((cpu->opcode >> 3) & 7);

    switch (cpu->opcode >> 6) {
        case 0:
            // Bit-shift ops
            switch (cpu->opcode >> 3) {
                case 0:
                    cpu_run_bitshift_op(cpu, target, bit_rlc);
                    return;
                case 1:
                    cpu_run_bitshift_op(cpu, target, bit_rrc);
                    return;
                case 2:
                    cpu_run_bitshift_op(cpu, target, bit_rl);
                    return;
                case 3:
                    cpu_run_bitshift_op(cpu, target, bit_rr);
                    return;
                case 4:
                    cpu_run_bitshift_op(cpu, target, bit_sla);
                    return;
                case 5:
                    cpu_run_bitshift_op(cpu, target, bit_sra);
                    return;
                case 6:
                    cpu_run_bitshift_op(cpu, target, bit_swap);
                    return;
                case 7:
                    cpu_run_bitshift_op(cpu, target, bit_srl);
                    return;
                default:
                    // unreachable
                    assert(false);
                    return;
            }
        case 1: {
            // Check bit
            bool is_zero = (cpu_get(cpu, target) & bitmask) == 0;
            cpu->f = (is_zero ? FZ : 0) | FH | (cpu->f & FC);
            return;
        }
        case 2: {
            // Reset bit
            cpu_set(cpu, target, cpu_get(cpu, target) & (~bitmask));
            return;
        }
        case 3: {
            // Set bit
            cpu_set(cpu, target, cpu_get(cpu, target) | bitmask);
            return;
        }
        default:
            // unreachable
            assert(false);
            return;
    }
}

void daa(u8 *value, u8 *flags) {
    // Z is set as normal
    // N is unchanged
    // H flag is always cleared
    // C is potentially set during this function

    if (*flags & FN) {
        // After subtraction, adjust if carry/half-carry occurred
        if (*flags & FC) *value -= 0x60;
        if (*flags & FH) *value -= 6;
    } else {
        // After addition, adjust if carry/half-carry occurred, or if value
        // is out of bounds
        if (*flags & FC || *value > 0x99) {
            *value += 0x60;
            *flags |= FC;
        }
        if (*flags & FH || (*value & 0x0F) > 0x09) *value += 6;
    }

    *flags = (*flags & (FC | FN)) | chk_z(*value);
}

void cpu_init(Cpu *cpu) {
    cpu->a = 0;
    cpu->f = 0;
    cpu->b = 0;
    cpu->c = 0;
    cpu->d = 0;
    cpu->e = 0;
    cpu->h = 0;
    cpu->l = 0;
    cpu->pc = 0;
    cpu->sp = 0;

    cpu->ime = true;
    cpu->halted = false;
    cpu->cycle = 0;

    cpu->bus_reg = 0;
    cpu->jp_lo = 0;
    cpu->jp_hi = 0;
    cpu->opcode = 0;
    cpu->ucode_step = 0;
}

void cpu_init_post_boot_dmg(Cpu *cpu) {
    cpu->a = 0x01;
    cpu->f = FZ;
    cpu->b = 0x00;
    cpu->c = 0x13;
    cpu->d = 0x00;
    cpu->e = 0xD8;
    cpu->h = 0x01;
    cpu->l = 0x4D;
    cpu->pc = 0x0100;
    cpu->sp = 0xFFFE;

    cpu->ime = true;
    cpu->halted = false;
    cpu->cycle = 0;

    cpu->bus_reg = 0;
    cpu->jp_lo = 0;
    cpu->jp_hi = 0;
    cpu->opcode = 0;
    cpu->ucode_step = 0;
}

void cpu_cycle(Cpu *cpu, Bus *bus) {
    assert(cpu);
    if (cpu->halted) return;

    if (cpu->ucode_step == 0) {
        // Read next opcode
        cpu->opcode = bus_read(bus, cpu->pc++);

        // Reset intra-micro-instruction registers to "uninitialized" state
        cpu->jp_lo = 0xAA;
        cpu->jp_hi = 0xAA;
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
        case WRITE_BC:
        case WRITE_DE:
        case WRITE_HL:
        case WRITE_HL_INC:
        case WRITE_HL_DEC:
        case WRITE_SP_DEC:
        case WRITE_JP_INC:
        case WRITE_FF00_C:
            break;
        case READ_PC_INC:
            cpu->bus_reg = bus_read(bus, cpu->pc++);
            break;
        case READ_BC:
            cpu->bus_reg = bus_read(bus, cpu_bc(cpu));
            break;
        case READ_DE:
            cpu->bus_reg = bus_read(bus, cpu_de(cpu));
            break;
        case READ_HL:
            cpu->bus_reg = bus_read(bus, cpu_hl(cpu));
            break;
        case READ_HL_INC:
            cpu->bus_reg = bus_read(bus, cpu_hl_postinc(cpu));
            break;
        case READ_HL_DEC:
            cpu->bus_reg = bus_read(bus, cpu_hl_postdec(cpu));
            break;
        case READ_SP_INC:
            cpu->bus_reg = bus_read(bus, cpu->sp++);
            break;
        case READ_JP:
            cpu->bus_reg = bus_read(bus, cpu_jp_reg(cpu));
            break;
        case READ_FF00_C:
            cpu->bus_reg = bus_read(bus, 0xFF00 + cpu->c);
            break;
        default:
            panicf("Unhandled input case: %d", uinst->io);
    }

    // Section 2 - ALU Op

    switch (uinst->uop) {
        case UOP_NONE:
            break;
        case LD_R8_R8:
            cpu_set(cpu, uinst->lhs, cpu_get(cpu, uinst->rhs));
            break;
        case INC16: {
            u16 x =
                to_u16(cpu_get(cpu, uinst->lhs), cpu_get(cpu, uinst->rhs)) + 1;
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            cpu_set(cpu, uinst->lhs, high_byte(x));
            cpu_set(cpu, uinst->rhs, low_byte(x));
            break;
        }
        case DEC16: {
            u16 x =
                to_u16(cpu_get(cpu, uinst->lhs), cpu_get(cpu, uinst->rhs)) - 1;
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            cpu_set(cpu, uinst->lhs, high_byte(x));
            cpu_set(cpu, uinst->rhs, low_byte(x));
            break;
        }
        case ADD16_LO: {
            // Preserve state of zero flag
            u8 orig_z = cpu->f & FZ;
            // Add
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, false);
            cpu->f = orig_z | (cpu->f & ~FZ);
            break;
        }
        case ADD16_HI: {
            // Preserve state of zero flag
            u8 orig_z = cpu->f & FZ;
            // Add with carry
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, true);
            cpu->f = orig_z | (cpu->f & ~FZ);
            break;
        }
        case ADD16_SP_I8:
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            alu_u16_plus_i8(cpu->sp, cpu_get(cpu, uinst->lhs), &cpu->sp,
                            &cpu->f);
            break;
        case ADD16_HL_SP_PLUS_I8: {
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            u16 hl;
            alu_u16_plus_i8(cpu->sp, cpu_get(cpu, uinst->lhs), &hl, &cpu->f);
            split_u16(hl, &cpu->h, &cpu->l);
            break;
        }
        case INC: {
            u8 lhs_val = cpu_get(cpu, uinst->lhs);
            alu_inc(lhs_val, &lhs_val, &cpu->f);
            cpu_set(cpu, uinst->lhs, lhs_val);
        } break;
        case DEC: {
            u8 lhs_val = cpu_get(cpu, uinst->lhs);
            alu_dec(lhs_val, &lhs_val, &cpu->f);
            cpu_set(cpu, uinst->lhs, lhs_val);
        } break;
        case ADD:
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, false);
            break;
        case ADC:
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, true);
            break;
        case SUB:
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, true, false);
            break;
        case SBC:
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, true, true);
            break;
        case AND:
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, bitwise_and);
            break;
        case XOR:
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, bitwise_xor);
            break;
        case OR:
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, bitwise_or);
            break;
        case CP:
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, bitwise_cp);
            break;
        case HALT:
            cpu->halted = true;
            break;
        case JP_REL:
            // Extend sign bit to jp_hi
            cpu->jp_hi = cpu->jp_lo & 0x80 ? 0xFF : 0;
            cpu->pc = cpu_jp_reg(cpu);
            break;
        case JP:
            cpu->pc = cpu_jp_reg(cpu);
            break;
        case JP_HL:
            cpu->pc = cpu_hl(cpu);
            break;
        case RST:
            cpu->pc = cpu->opcode & 0x38;
            break;
        case RLCA:
            cpu_run_bitshift_op(cpu, A, bit_rlc);
            break;
        case RRCA:
            cpu_run_bitshift_op(cpu, A, bit_rrc);
            break;
        case RLA:
            cpu_run_bitshift_op(cpu, A, bit_rl);
            break;
        case RRA:
            cpu_run_bitshift_op(cpu, A, bit_rr);
            break;
        case PREFIX_OP:
            cpu_run_prefix_op(cpu, uinst->lhs);
            break;
        case DAA: {
            daa(&cpu->a, &cpu->f);
            break;
        }
        case CPL:
            cpu->a ^= 0xFF;
            cpu->f |= FN | FH;
            break;
        case SCF:
            cpu->f &= ~(FN | FH);
            cpu->f |= FC;
            break;
        case CCF:
            cpu->f &= ~(FN | FH);
            cpu->f ^= FC;
            break;
        case DISABLE_INTERRUPTS:
            cpu->ime = false;
            break;
        case ENABLE_INTERRUPTS:
            cpu->ime = true;
            break;
        default:
            panicf("Unhandled micro-op case: %d", uinst->uop);
    }

    // Section 3 - Bus Write

    switch (uinst->io) {
        case IO_NONE:
        case READ_PC_INC:
        case READ_BC:
        case READ_DE:
        case READ_HL:
        case READ_HL_INC:
        case READ_HL_DEC:
        case READ_SP_INC:
        case READ_JP:
        case READ_FF00_C:
            break;
        case WRITE_BC:
            bus_write(bus, cpu_bc(cpu), cpu->bus_reg);
            break;
        case WRITE_DE:
            bus_write(bus, cpu_de(cpu), cpu->bus_reg);
            break;
        case WRITE_HL:
            bus_write(bus, cpu_hl(cpu), cpu->bus_reg);
            break;
        case WRITE_HL_INC:
            bus_write(bus, cpu_hl_postinc(cpu), cpu->bus_reg);
            break;
        case WRITE_HL_DEC:
            bus_write(bus, cpu_hl_postdec(cpu), cpu->bus_reg);
            break;
        case WRITE_SP_DEC:
            bus_write(bus, --cpu->sp, cpu->bus_reg);
            break;
        case WRITE_JP_INC:
            bus_write(bus, cpu_jp_reg_postinc(cpu), cpu->bus_reg);
            break;
        case WRITE_FF00_C:
            bus_write(bus, 0xFF00 + cpu->c, cpu->bus_reg);
            break;
        default:
            panicf("Unhandled output case: %d", uinst->io);
    }

    // Section 4 - Counters/Conditions

    if (instructions_is_last_ustep(cpu->opcode, cpu->ucode_step)) {
        cpu->ucode_step = 0;
    } else {
        ++cpu->ucode_step;
    }

    switch (uinst->cond) {
        case COND_ALWAYS:
            break;
        case COND_NZ:
            if ((cpu->f & FZ) != 0) cpu->ucode_step = 0;
            break;
        case COND_Z:
            if ((cpu->f & FZ) == 0) cpu->ucode_step = 0;
            break;
        case COND_NC:
            if ((cpu->f & FC) != 0) cpu->ucode_step = 0;
            break;
        case COND_C:
            if ((cpu->f & FC) == 0) cpu->ucode_step = 0;
            break;
        default:
            panicf("Unhandled condition case: %d", uinst->cond);
    }

    ++cpu->cycle;
}

void cpu_print_info(Cpu *cpu) {
    printf("CPU Cycle: %lld\n", cpu->cycle);
    printf("  A: %02X  F: %02X\n", cpu->a, cpu->f);
    printf("  B: %02X  C: %02X\n", cpu->b, cpu->c);
    printf("  D: %02X  E: %02X\n", cpu->d, cpu->e);
    printf("  H: %02X  L: %02X\n", cpu->h, cpu->l);
    printf("  SP: %04X\n", cpu->sp);
    printf("  PC: %04X\n", cpu->pc);
    printf("  tmp: %04X busreg: %02X\n", to_u16(cpu->jp_hi, cpu->jp_lo),
           cpu->bus_reg);
    printf("  op: %02X  step: %d\n", cpu->opcode, cpu->ucode_step);
}