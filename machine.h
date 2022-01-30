#ifndef _MACHINE_H_
#define _MACHINE_H_

#include "proc.h"
#include "mem.h"

// User/supervisor mode.
typedef enum {
    MODE_USER,
    MODE_KER,
    MODE_ERR = -1
} machine_mode_t;

// Machine state.
typedef struct machine {
    char *name;
    unsigned word_size;
    byte_order_t code_order;
    byte_order_t data_order;
    machine_mode_t mode;
    proc_t *proc;
    mem_t *mem;
} machine_t;

extern void init_machine(char *, unsigned, byte_order_t, byte_order_t);
#endif