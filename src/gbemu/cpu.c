#include "cpu.h"

#include <string.h>

#include "instructions.h"

#define FZ 0x80
#define FN 0x40
#define FH 0x20
#define FC 0x10

#define chk_z(x) ((x) == 0 ? FZ : 0)

typedef enum InterruptAddr {
    INTR_VBLANK_ADDR = 0x40,
    INTR_LCD_STAT_ADDR = 0x48,
    INTR_TIMER_ADDR = 0x50,
    INTR_SERIAL_ADDR = 0x58,
    INTR_JOYPAD_ADDR = 0x60,
} InterruptAddr;

// static u8 low_byte(u16 word) { return (u8)(word & 0xFF); }
// static u8 high_byte(u16 word) { return (u8)(word >> 8); }
// static u16 to_u16(u8 hi, u8 lo) { return (((u16)hi) << 8) | ((u16)lo); }
static void split_u16(u16 value, u8 *hi, u8 *lo) {
    *hi = high_byte(value);
    *lo = low_byte(value);
}

u16 cpu_af(const Cpu *cpu) { return to_u16(cpu->a, cpu->f); }
u16 cpu_bc(const Cpu *cpu) { return to_u16(cpu->b, cpu->c); }
u16 cpu_de(const Cpu *cpu) { return to_u16(cpu->d, cpu->e); }
u16 cpu_hl(const Cpu *cpu) { return to_u16(cpu->h, cpu->l); }

static u16 cpu_hl_postinc(Cpu *cpu) {
    u16 x = to_u16(cpu->h, cpu->l);
    split_u16(x + 1, &cpu->h, &cpu->l);
    return x;
}
static u16 cpu_hl_postdec(Cpu *cpu) {
    u16 x = to_u16(cpu->h, cpu->l);
    split_u16(x - 1, &cpu->h, &cpu->l);
    return x;
}
static u16 cpu_jp_reg(const Cpu *cpu) { return to_u16(cpu->jp_hi, cpu->jp_lo); }
static u16 cpu_jp_reg_postinc(Cpu *cpu) {
    u16 x = to_u16(cpu->jp_hi, cpu->jp_lo);
    split_u16(x + 1, &cpu->jp_hi, &cpu->jp_lo);
    return x;
}

static void cpu_set(Cpu *cpu, Target target, u8 value) {
    switch (target) {
        case TARGET_NONE:
            break;
        case A:
            cpu->a = value;
            break;
        case F:
            cpu->f = value & 0xF0;
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

static u8 cpu_get(const Cpu *cpu, Target target) {
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

static void alu_add(u8 lhs, u8 rhs, u8 carry, u8 *out, u8 *flags) {
    carry = carry ? 1 : 0;

    u8 res = lhs + rhs + carry;

    u8 z = chk_z(res);
    u8 h = ((lhs & 0xf) + (rhs & 0xf) + carry) > 0xf ? FH : 0;
    u8 c = (u16)res < (u16)rhs + (u16)carry ? FC : 0;

    *flags = z | h | c;
    *out = res;
}

static void alu_sub(u8 lhs, u8 rhs, u8 borrow, u8 *out, u8 *flags) {
    borrow = borrow ? 1 : 0;

    u8 res = lhs - rhs - borrow;

    u8 z = chk_z(res);
    u8 h = (((lhs & 0xf) - (rhs & 0xf) - borrow) & 0x10) != 0 ? FH : 0;
    u8 c = (u16)lhs < (u16)rhs + (u16)borrow ? FC : 0;

    *flags = z | FN | h | c;
    *out = res;
}

static void cpu_run_alu(Cpu *cpu, Target lhs, Target rhs, bool sub,
                        bool use_carry) {
    u8 lhs_val = cpu_get(cpu, lhs);
    u8 rhs_val = cpu_get(cpu, rhs);
    u8 carry_val = use_carry ? (cpu->f & FC) != 0 : 0;
    if (sub) {
        alu_sub(lhs_val, rhs_val, carry_val, &lhs_val, &cpu->f);
    } else {
        alu_add(lhs_val, rhs_val, carry_val, &lhs_val, &cpu->f);
    }
    cpu_set(cpu, lhs, lhs_val);
}

static void alu_add_u16_i8(u16 lhs16, u8 rhs, u16 *out, u8 *flags) {
    u16 rhs16 = (i16)((i8)rhs);
    *out = lhs16 + rhs16;

    u8 h = (lhs16 & 0xF) + (rhs16 & 0xF) > 0xF ? FH : 0;
    u8 c = (lhs16 & 0xFF) + (rhs16 & 0xFF) > 0xFF ? FC : 0;
    *flags = h | c;
}

static void alu_inc(u8 lhs, u8 *out, u8 *flags) {
    *out = lhs + 1;
    *flags = chk_z(*out) | ((lhs & 0xF) == 0xF ? FH : 0) | (*flags & FC);
}

static void alu_dec(u8 lhs, u8 *out, u8 *flags) {
    *out = lhs - 1;
    *flags = chk_z(*out) | FN | ((lhs & 0xF) == 0 ? FH : 0) | (*flags & FC);
}

static void bitwise_and(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    *out = lhs & rhs;
    *flags = chk_z(*out) | FH;
}

static void bitwise_xor(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    *out = lhs ^ rhs;
    *flags = chk_z(*out);
}

static void bitwise_or(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    *out = lhs | rhs;
    *flags = chk_z(*out);
}

static void alu_cp(u8 lhs, u8 rhs, u8 *out, u8 *flags) {
    // Same as SUB r8,r8 but result is ignored
    (void)out;
    u8 dummy;
    alu_sub(lhs, rhs, 0, &dummy, flags);
}

static void cpu_run_bitwise_op(Cpu *cpu, Target lhs, Target rhs,
                               void (*bitwise_op)(u8, u8, u8 *, u8 *)) {
    u8 lhs_val = cpu_get(cpu, lhs);
    u8 rhs_val = cpu_get(cpu, rhs);
    bitwise_op(lhs_val, rhs_val, &lhs_val, &cpu->f);
    cpu_set(cpu, lhs, lhs_val);
}

static void bit_rlc(u8 *value, u8 *flags) {
    *value = to_u16(*value, *value) >> 7;
    *flags = chk_z(*value) | (*value & 1 ? FC : 0);
}

void bit_rrc(u8 *value, u8 *flags) {
    *value = to_u16(*value, *value) >> 1;
    *flags = chk_z(*value) | (*value & 0x80 ? FC : 0);
}

static void bit_rl(u8 *value, u8 *flags) {
    bool new_carry = *value & 0x80;
    // Move carry flag to bit 7 so that it is shifted into 0
    *value = to_u16(*value, *flags << 3) >> 7;
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

static void bit_rr(u8 *value, u8 *flags) {
    bool new_carry = *value & 1;
    // Move carry flag to bit 9 so that it is shifted into 8
    *value = to_u16(*flags >> 4, *value) >> 1;
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

static void bit_sla(u8 *value, u8 *flags) {
    bool new_carry = *value & 0x80;
    *value <<= 1;
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

static void bit_sra(u8 *value, u8 *flags) {
    bool new_carry = *value & 1;
    // Arithmetic shift: sign bit is preserved
    *value = (*value & 0x80) | (*value >> 1);
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

static void bit_srl(u8 *value, u8 *flags) {
    bool new_carry = *value & 1;
    // Logical shift: sign bit is removed
    *value >>= 1;
    assert((*value & 0x80) == 0);
    *flags = chk_z(*value) | (new_carry ? FC : 0);
}

static void bit_swap(u8 *value, u8 *flags) {
    *value = (*value << 4) | (*value >> 4);
    *flags = chk_z(*value);
}

static void cpu_run_bitshift_op(Cpu *cpu, Target target,
                                void (*bitshift_op)(u8 *, u8 *)) {
    u8 value = cpu_get(cpu, target);
    bitshift_op(&value, &cpu->f);
    cpu_set(cpu, target, value);
}

static Target get_prefix_op_target(u8 prefix_op) {
    switch (prefix_op & 7) {
        case 0:
            return B;
        case 1:
            return C;
        case 2:
            return D;
        case 3:
            return E;
        case 4:
            return H;
        case 5:
            return L;
        case 6:
            return BUS;
        case 7:
            return A;
    }
    assert(0);
    return TARGET_NONE;
}

static void cpu_run_prefix_op(Cpu *cpu, u8 prefix_op, Target target) {
    u8 bitmask = 1 << ((prefix_op >> 3) & 7);

    switch (prefix_op >> 6) {
        case 0:
            // Bit-shift ops
            switch (prefix_op >> 3) {
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

static void daa(u8 *value, u8 *flags) {
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

        // Implementations vary on if this should be 0x99 or 0x9F
        if (*flags & FC || *value > 0x99) {
            *value += 0x60;
            *flags |= FC;
        }
        if (*flags & FH || (*value & 0x0F) > 0x09) *value += 6;
    }

    *flags = (*flags & (FC | FN)) | chk_z(*value);
}

static bool cpu_check_jp_interrupt(Cpu *cpu, Bus *bus, u8 mask, u16 address) {
    if ((bus->high_byte_ram.data[FF_IF] & bus->high_byte_ram.data[FF_IE] &
         mask) == 0)
        return false;

    bus->high_byte_ram.data[FF_IF] &= ~mask;

    // Save address to call into. (See `interrupt_call_instruction[]`)
    split_u16(address, &cpu->jp_hi, &cpu->jp_lo);

    cpu->is_jp_interrupt = true;
    cpu->halted = false;
    cpu->ime = false;

    return true;
}

// Same as CALL instruction, but without the initial (PC++) reads.
const MicroInstr interrupt_call_instruction[MICRO_INSTRUCTION_SIZE] = {
    {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_HI, .io = WRITE_SP_DEC},
    {.uop = LD_R8_R8, .lhs = BUS, .rhs = PC_LO, .io = WRITE_SP_DEC},
    {.uop = JP, .end = true},
};

void cpu_reset(Cpu *cpu) {
    assert(cpu);
    memset(cpu, 0, sizeof(Cpu));

    cpu->ime = true;
}

void cpu_cycle(Cpu *cpu, Bus *bus) {
    assert(cpu);

    // Section 1 - Bus Read

    if (cpu->ucode_step == 0) {
        // Reset intra-micro-instruction registers to "uninitialized" state
        cpu->is_jp_interrupt = false;
        cpu->jp_lo = 0xAA;
        cpu->jp_hi = 0xAA;
        cpu->bus_reg = 0xAA;

        // Check if any of the interrupt registers flags (bits 0-5) are both
        // enabled (IE) and requested (IF).
        bool interrupt_requested = bus->high_byte_ram.data[FF_IF] &
                                   bus->high_byte_ram.data[FF_IE] & 0x1F;

        // If halted, wait until interrupt
        if (cpu->halted) {
            // TODO: "halt bug": https://gbdev.io/pandocs/halt.html
            if (interrupt_requested) {
                cpu->halted = false;
            } else {
                return;
            }
        };

        // Handle interrupts
        if (cpu->ime && interrupt_requested) {
            // Check each interrupt for a jump in priority order

            do {
                if (cpu_check_jp_interrupt(cpu, bus, INTR_VBLANK_MASK,
                                           INTR_VBLANK_ADDR)) {
                    break;
                }

                if (cpu_check_jp_interrupt(cpu, bus, INTR_LCD_STAT_MASK,
                                           INTR_LCD_STAT_ADDR)) {
                    break;
                }

                if (cpu_check_jp_interrupt(cpu, bus, INTR_TIMER_MASK,
                                           INTR_TIMER_ADDR)) {
                    break;
                }

                if (cpu_check_jp_interrupt(cpu, bus, INTR_SERIAL_MASK,
                                           INTR_SERIAL_ADDR)) {
                    break;
                }

                if (cpu_check_jp_interrupt(cpu, bus, INTR_JOYPAD_MASK,
                                           INTR_JOYPAD_ADDR)) {
                    break;
                }

                // All cases should have been covered
                assert(false);
            } while (0);
        }

        // IE sets IME after 1 instruction delay
        if (cpu->ime_scheduled) {
            cpu->ime_scheduled = false;
            cpu->ime = true;
        }

        // Read next opcode (if not calling interrupt)
        if (!cpu->is_jp_interrupt) {
            cpu->opcode = bus_read(bus, cpu->pc++);
        }
    }

    const MicroInstr *uinst =
        cpu->is_jp_interrupt
            ? &interrupt_call_instruction[cpu->ucode_step]
            : instructions_get_uinst(cpu->opcode, cpu->ucode_step);

    // Flag for if this is the last micro-instruction.
    // (Can be overridden by 0xCB)
    bool is_end = uinst->end;

    if (uinst->undefined) {
        panicf("Executing undefined instr: $%02X (step %d)", cpu->opcode,
               cpu->ucode_step);
    }

    // Make sure we aren't doing multiple bus interactions on the same step
    assert(cpu->is_jp_interrupt || cpu->ucode_step != 0 ||
           uinst->io == IO_NONE);

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
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_set(cpu, uinst->lhs, cpu_get(cpu, uinst->rhs));
            break;
        case INC16: {
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            u16 x =
                to_u16(cpu_get(cpu, uinst->lhs), cpu_get(cpu, uinst->rhs)) + 1;
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            cpu_set(cpu, uinst->lhs, high_byte(x));
            cpu_set(cpu, uinst->rhs, low_byte(x));
            break;
        }
        case DEC16: {
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            u16 x =
                to_u16(cpu_get(cpu, uinst->lhs), cpu_get(cpu, uinst->rhs)) - 1;
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            cpu_set(cpu, uinst->lhs, high_byte(x));
            cpu_set(cpu, uinst->rhs, low_byte(x));
            break;
        }
        case ADD16_LO: {
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            // Preserve state of zero flag
            u8 orig_z = cpu->f & FZ;
            // Add
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, false);
            cpu->f = orig_z | (cpu->f & ~FZ);
            break;
        }
        case ADD16_HI: {
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            // Preserve state of zero flag
            u8 orig_z = cpu->f & FZ;
            // Add with carry
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, true);
            cpu->f = orig_z | (cpu->f & ~FZ);
            break;
        }
        case ADD16_SP_I8:
            assert(uinst->lhs != TARGET_NONE);
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            alu_add_u16_i8(cpu->sp, cpu_get(cpu, uinst->lhs), &cpu->sp,
                           &cpu->f);
            break;
        case ADD16_HL_SP_PLUS_I8: {
            assert(uinst->lhs != TARGET_NONE);
            // Two register sets in the same cycle is probably not technically
            // accurate, but it shouldn't make a difference
            u16 hl;
            alu_add_u16_i8(cpu->sp, cpu_get(cpu, uinst->lhs), &hl, &cpu->f);
            split_u16(hl, &cpu->h, &cpu->l);
            break;
        }
        case INC: {
            assert(uinst->lhs != TARGET_NONE);
            u8 lhs_val = cpu_get(cpu, uinst->lhs);
            alu_inc(lhs_val, &lhs_val, &cpu->f);
            cpu_set(cpu, uinst->lhs, lhs_val);
        } break;
        case DEC: {
            assert(uinst->lhs != TARGET_NONE);
            u8 lhs_val = cpu_get(cpu, uinst->lhs);
            alu_dec(lhs_val, &lhs_val, &cpu->f);
            cpu_set(cpu, uinst->lhs, lhs_val);
        } break;
        case ADD:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, false);
            break;
        case ADC:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, false, true);
            break;
        case SUB:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, true, false);
            break;
        case SBC:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_alu(cpu, uinst->lhs, uinst->rhs, true, true);
            break;
        case AND:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, bitwise_and);
            break;
        case XOR:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, bitwise_xor);
            break;
        case OR:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, bitwise_or);
            break;
        case CP:
            assert(uinst->lhs != TARGET_NONE);
            assert(uinst->rhs != TARGET_NONE);
            cpu_run_bitwise_op(cpu, uinst->lhs, uinst->rhs, alu_cp);
            break;
        case HALT:
            cpu->halted = true;
            break;
        case JP_REL:
            // Extend sign bit to jp_hi
            cpu->jp_hi = cpu->jp_lo & 0x80 ? 0xFF : 0;
            cpu->pc += cpu_jp_reg(cpu);
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
            cpu->f &= FC;
            break;
        case RRCA:
            cpu_run_bitshift_op(cpu, A, bit_rrc);
            cpu->f &= FC;
            break;
        case RLA:
            cpu_run_bitshift_op(cpu, A, bit_rl);
            cpu->f &= FC;
            break;
        case RRA:
            cpu_run_bitshift_op(cpu, A, bit_rr);
            cpu->f &= FC;
            break;
        case PREFIX_OP_1: {
            // Register r8 Prefix Op
            // Asserts for only supported use-cases

            assert(uinst->lhs == BUS);
            cpu->prefix_opcode = cpu_get(cpu, uinst->lhs);

            Target prefix_target = get_prefix_op_target(cpu->prefix_opcode);

            if (prefix_target != BUS) {
                cpu_run_prefix_op(cpu, cpu->prefix_opcode, prefix_target);
                is_end = true;
            }
            break;
        }
        case PREFIX_OP_2: {
            // Memory (HL) Prefix Op
            // Asserts for only supported use-cases

            assert(uinst->lhs == BUS);
            Target prefix_target = get_prefix_op_target(cpu->prefix_opcode);
            assert(prefix_target == BUS);
            cpu_run_prefix_op(cpu, cpu->prefix_opcode, prefix_target);
            assert(uinst->io == WRITE_HL);
            break;
        }
        case DAA:
            daa(&cpu->a, &cpu->f);
            break;
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
        case ENABLE_INTERRUPTS_DELAYED:
            cpu->ime_scheduled = true;
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

    if (is_end) {
        cpu->ucode_step = 0;
    } else {
        ++cpu->ucode_step;
    }
    assert(cpu->ucode_step < MICRO_INSTRUCTION_SIZE);

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

    // Should never write to lower nibble of F
    assert((cpu->f & 0x0F) == 0);
}

void cpu_print_trace(const Cpu *cpu, const Bus *bus) {
    u8 pc0 = bus_peek(bus, cpu->pc);
    const char *mnemonic = instructions_get_mnemonic(pc0);
    u8 len = instructions_get_length(pc0);

    printf("%04X: %02X", cpu->pc, pc0);

    if (len > 1) {
        u8 pc1 = bus_peek(bus, cpu->pc + 1);

        if (pc0 == 0xCB) {
            mnemonic = instructions_get_prefix_mnemonic(pc1);
        }

        printf(" %02X", pc1);
    } else {
        printf("   ");
    }
    if (len > 2) {
        u8 pc2 = bus_peek(bus, cpu->pc + 2);
        printf(" %02X", pc2);
    } else {
        printf("   ");
    }

    char z = (cpu->f & FZ) ? 'Z' : '-';
    char n = (cpu->f & FN) ? 'N' : '-';
    char h = (cpu->f & FH) ? 'H' : '-';
    char c = (cpu->f & FC) ? 'C' : '-';

    printf(" ; %-14s| A:%02X F:%c%c%c%c", mnemonic, cpu->a, z, n, h, c);

    // printf(
    //     " BC:%02X%02X DE:%02X%02X HL:%02X%02X "
    //     "SP:%04X",
    //     cpu->b, cpu->c, cpu->d, cpu->e, cpu->h, cpu->l, cpu->sp);

#define print_addr2(name, reg1, reg2)                        \
    printf(" " #name ":%02X%02X", bus_peek(bus, (0x##reg1)), \
           bus_peek(bus, (0x##reg2)))
#define print_addr(name, reg) \
    printf(" " #name ":%02X", bus_peek(bus, (0x##reg)))

    // print_addr(DMA, FF46);
    // print_addr2(DIV, FF04, FF03);
    // print_addr(TIMA, FF05);
    // print_addr(TMA, FF06);
    // print_addr(TAC, FF07);
    print_addr(LCDC, FF40);
    print_addr(STAT, FF41);
    print_addr(LY, FF44);
    print_addr(LYC, FF45);
    print_addr(IF, FF0F);
    print_addr(IE, FFFF);

#undef print_addr
#undef print_addr2

    if (cpu->halted) {
        printf(" HALT");
    }

    printf("\n");
}

// void cpu_print_info(const Cpu *cpu) {
//     printf("CPU Cycle: %lld\n", cpu->cycle);
//     printf("  A: %02X  F: %02X\n", cpu->a, cpu->f);
//     printf("  B: %02X  C: %02X\n", cpu->b, cpu->c);
//     printf("  D: %02X  E: %02X\n", cpu->d, cpu->e);
//     printf("  H: %02X  L: %02X\n", cpu->h, cpu->l);
//     printf("  SP: %04X\n", cpu->sp);
//     printf("  PC: %04X\n", cpu->pc);
//     printf("  tmp: %04X busreg: %02X\n", to_u16(cpu->jp_hi, cpu->jp_lo),
//            cpu->bus_reg);
//     printf("  op: %02X  step: %d\n", cpu->opcode, cpu->ucode_step);
// }