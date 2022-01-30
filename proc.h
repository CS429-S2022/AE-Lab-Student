#ifndef _PROC_H_
#define _PROC_H_
#include <stdint.h>
#include "reg.h"

// Processor state.
typedef struct proc {
    reg_file_t GPR;
    // reg_file_t FPR;
    reg_t PC;
    reg_t SP;
    reg_t NZCV;
} proc_t;

extern int runElf(const uint64_t);
#endif