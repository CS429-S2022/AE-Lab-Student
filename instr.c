#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "err_handler.h"
#include "instr.h"
#include "machine.h"

#define EXTRACT(src, mask, shift) (((src) & (mask)) >> (shift))
#define GETBF(src, frompos, width) (((src) & (((1 << (width)) - 1) << (frompos))) >> (frompos))

extern machine_t guest;

static char *opcode_names[] = {"ERR", "ADD", "ADRP", "MOV", "MOVZ"};

static char *cond_names[] = {
    "EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC", 
    "HI", "LS", "GE", "LT", "GT", "LE", "AL", "NV"
};

// Main: C6.2.4 ADD (immediate)
// Alias: C6.2.185 MOV (to/from SP)
static void decode_ADD_IMM_64(const uint32_t instr, instr_t * const insn) {
    assert(EXTRACT(instr, 0xFF800000, 23) == 0x122U);

    uint8_t d = EXTRACT(instr, 0x1FU, 0);
    uint8_t n = EXTRACT(instr, 0x3E0U, 5);
    uint16_t imm12 = EXTRACT(instr, 0x3FFC00U, 10);
    uint8_t sh = EXTRACT(instr, 400000U, 22);
    bool is_aliased = (sh == 0 && imm12 == 0 && (d == 31 || n == 31));

    insn->op = is_aliased ? OP_MOV : OP_ADD;
    insn->var = WVAR_64;
    insn->dst = (d == 31) ? &(guest.proc->SP) : (guest.proc->GPR.regs + d);
    insn->src1 = (n == 31) ? &(guest.proc->SP) : (guest.proc->GPR.regs + n);
    insn->imm = sh ? imm12 << 12 : imm12;
}

static void execute_ADD_IMM_64(instr_t * const insn) {
    insn->dst->value = insn->src1->value + insn->imm;
}

// Main: C6.2.11 ADRP
static void decode_ADRP(const uint32_t instr, instr_t * const insn) {
    assert(EXTRACT(instr, 0x80000000U, 31) == 0x1U);
    assert(EXTRACT(instr, 0x1F000000U, 24) == 0x10U);
    
    uint8_t d = EXTRACT(instr, 0x1FU, 0);
    int8_t immlo = EXTRACT(instr, 0x60000000U, 29);
    int32_t immhi = EXTRACT(instr, 0x00FFFFE0, 5);
    int64_t imm = ((int64_t) ((immhi << 2) | immlo)) << 12;// FIX

    insn->op = OP_ADRP;
    insn->var = WVAR_64;
    insn->dst = guest.proc->GPR.regs + d;
    insn->imm = imm;
}

static void execute_ADRP(instr_t * const insn) {
    uint64_t base = guest.proc->PC.value;
    base &= 0xFFFFFFFFFFFFF000ULL;
    insn->dst->value = base + insn->imm;
}

// Main: C6.2.192 MOVZ
// Alias: C6.2.187 MOV (wide immediate)
static void decode_MOVZ_32(const uint32_t instr, instr_t * const insn) {
    assert(EXTRACT(instr, 0xFFC00000U, 22) == 0x14AU);

    uint8_t d = EXTRACT(instr, 0x1FU, 0);
    uint16_t imm16 = EXTRACT(instr, 0x1FFFE0U, 5);
    uint8_t hw = EXTRACT(instr, 0x600000U, 21);
    uint8_t pos = hw << 4;
    bool is_aliased = ! ((imm16 == 0) && (hw != 0));

    insn->op = is_aliased ? OP_MOV : OP_MOVZ;
    insn->var = WVAR_32;
    insn->dst = guest.proc->GPR.regs + d;
    insn->imm = imm16;
    insn->shift = pos;
}

// PC-rel. addressing (DPI/0, DPI/1)
static void decode_PC_REL_ADDR(const uint32_t instr, instr_t * const insn) {
    assert(EXTRACT(instr, 0x10000000U, 24) == 0x10U);
    
    unsigned char op = EXTRACT(instr, 0x80000000U, 31);
    switch (op) {
        case 0: MISSING(); break;
        case 1: decode_ADRP(instr, insn); break;
        default: assert(false); break;
    }
}

// Add/subtract (immediate) (DPI/2)
static void decode_ADD_SUB_IMM(const uint32_t instr, instr_t * const insn) {
    assert(EXTRACT(instr, 0x1F800000U, 23) == 0x22U);
    
    unsigned char sf_op_S = EXTRACT(instr, 0xE0000000U, 29);
    switch (sf_op_S) {
        case 0: MISSING(); break;
        case 1: MISSING(); break;
        case 2: MISSING(); break;
        case 3: MISSING(); break;
        case 4: decode_ADD_IMM_64(instr, insn); break;
        case 5: MISSING(); break;
        case 6: MISSING(); break;
        case 7: MISSING(); break;
        default: assert(false); break;
    }
}

// Move wide (immediate) (DPI/5)
static void decode_MOVE_WIDE_IMM(const uint32_t instr, instr_t * const insn) {
    assert(EXTRACT(instr, 0x1F800000U, 23) == 0x25U);
    
    unsigned char sf_opc = EXTRACT(instr, 0xE0000000, 29);
    unsigned char hw = EXTRACT(instr, 0x600000U, 21);
    switch (sf_opc) {
        case 0: MISSING(); break;
        case 1: MISSING(); break;
        case 2: decode_MOVZ_32(instr, insn); break;
        case 3: MISSING(); break;
        case 4: MISSING(); break;
        case 5: MISSING(); break;
        case 6: MISSING(); break;
        case 7: MISSING(); break;
        default: assert(false); break;
    }
}

// Load/store register (unsigned immediate) (p. C4-309)
static void decode_LD_ST_REG_U_IMM(const uint32_t instr, instr_t * const insn) {
}

// C4.1.2 Data Processing -- Immediate
static void decode_DPI(const uint32_t instr, instr_t * const insn) {
    assert(EXTRACT(instr, 0x1C000000U, 26) == 0x4U);
    
    unsigned char op0 = EXTRACT(instr, 0x03800000U, 23);
    switch (op0) {
        case 0: case 1: decode_PC_REL_ADDR(instr, insn); break;
        case 2: decode_ADD_SUB_IMM(instr, insn); break;
        case 3: MISSING(); break;
        case 4: MISSING(); break;
        case 5: decode_MOVE_WIDE_IMM(instr, insn); break;
        case 6: MISSING(); break;
        case 7: MISSING(); break;
        default: assert(false); break;
    }
}

// C4.1.4 Loads and Stores
static void decode_LD_ST(const uint32_t instr, instr_t * const insn) {
    unsigned char op0 = EXTRACT(instr, 0xF0000000U, 28);
    unsigned char op1 = EXTRACT(instr, 0x8000000U, 27);
    unsigned char op2 = EXTRACT(instr, 0x1800000U, 23);
    unsigned char op3 = EXTRACT(instr, 0x3F0000U, 16);
    unsigned char op4 = EXTRACT(instr, 0xC00U, 10);

    switch (op0) {
        case 0: case 4: MISSING(); break;
        case 13: MISSING(); break;
        case 8: case 12: MISSING(); break;
        case 1: case 5: case 9: MISSING(); break;
        case 2: case 6: case 10: case 14: MISSING(); break;
        case 3: case 7: case 11: case 15: 
            switch (op2) {
                case 0: case 1: MISSING(); break;
                case 2: case 3: decode_LD_ST_REG_U_IMM(instr, insn); break;
            }
            break;
        default: assert(false); break;
    }
}

instr_t *decode_instr(const uint32_t instr) {
    instr_t *insn = calloc(1, sizeof(instr_t));
    // bits 28:25 (C4.1)
    unsigned char op0 = EXTRACT(instr, 0x1E000000U, 25);
    switch (op0) {
        case 0: MISSING(); break;
        case 1: break;
        case 2: MISSING(); break;
        case 3: break;
        case 8: case 9: decode_DPI(instr, insn); break;
        case 10: case 11: MISSING(); break;
        case 4: case 6: case 12: case 14: decode_LD_ST(instr, insn); break;
        case 5: case 13: MISSING(); break;
        case 7: case 15: MISSING(); break;
        default: assert(false); break;
    }
    return insn;
}

void show_instr(const instr_t * insn) {
    printf("[op = %s,\tcond = %s,\tdst = %s%s,\tsrc1 = %s%s,\tsrc2 = %s%s,\timm = 0x%08lx,\tshift = %d]\n", 
        opcode_names[insn->op], 
        insn->cond ? cond_names[insn->cond] : "---",
        insn->dst ? reg_prefix[insn->var] : "", 
        insn->dst ? insn->dst->name : "---",
        insn->src1 ? reg_prefix[insn->var] : "", 
        insn->src1 ? insn->src1->name : "---",
        insn->src2 ? reg_prefix[insn->var] : "", 
        insn->src2 ? insn->src2->name : "---",
        insn->imm, 
        insn->shift);
}