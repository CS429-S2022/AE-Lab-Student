#ifndef _REG_H_
#define _REG_H_

typedef struct reg {
    char *name;
    unsigned index;
    unsigned width;
    long long value;// FIX
} reg_t;

typedef struct reg_file {
    char *name;
    unsigned num;
    unsigned width;
    reg_t *regs;
} reg_file_t;

extern char *reg_prefix[];

extern void init_reg(reg_t *r, char *name, unsigned index, unsigned width);
extern void init_reg_file(reg_file_t *rf, char *name, unsigned num, unsigned width);
#endif