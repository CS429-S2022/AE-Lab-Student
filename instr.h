#ifndef _INSTR_H_
#define _INSTR_H_

#include <stdint.h>
#include "reg.h"

typedef enum {
    OP_NONE,
    OP_ADD,
    OP_ADRP,
    OP_MOV,
    OP_MOVZ,
    OP_ERROR = -1
} opcode_t;

typedef enum {
    C_EQ,
    C_NE,
    C_CS,
    C_CC,
    C_MI,
    C_PL,
    C_VS,
    C_VC,
    C_HI,
    C_LS,
    C_GE,
    C_LT,
    C_GT,
    C_LE,
    C_AL,
    C_NV,
    C_ERROR = -1
} cond_t;

typedef enum {
    WVAR_32,
    WVAR_64,
    WVAR_ERROR = -1
} wvar_t;

typedef struct instr {
    opcode_t op;
    wvar_t var;
    cond_t cond;
    reg_t *dst;
    reg_t *src1;
    reg_t *src2;
    int64_t imm;
    uint8_t shift;
    uint64_t next_PC;
    uint64_t branch_PC;
} instr_t;

extern instr_t *decode_instr(const uint32_t);
extern void show_instr(const instr_t *);
#endif