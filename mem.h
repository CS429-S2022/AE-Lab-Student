#ifndef _MEM_H_
#define _MEM_H_

#include <stdint.h>
// Memory state.
typedef enum {
    L_ENDIAN,
    B_ENDIAN,
    ERROR_ENDIAN = -1
} byte_order_t;

typedef enum {
    BYTE_GRAN,
    ERROR_GRAN = -1
} mem_gran_t;

typedef enum {
    NOACCESS_SEG = 0,
    TEXT_SEG,
    DATA_SEG,
    HEAP_SEG,
    SO_SEG,
    STACK_SEG,
    KERNEL_SEG,
    ERROR_SEG = -1
} seg_t;

typedef struct mem {
    unsigned long long max_addr;
    unsigned addr_size;
    mem_gran_t gran;
    uint64_t seg_start_addr[KERNEL_SEG+1];
    uint8_t seg_prot[KERNEL_SEG+1];
} mem_t;

// Return value read from address.
extern char      mem_read_B (uint64_t address);
extern short     mem_read_S (uint64_t address);
extern int       mem_read_I (uint64_t address);
extern long      mem_read_L (uint64_t address);
extern long long mem_read_LL(uint64_t address);

typedef enum {
    WRITE_SUCCESS,
    WRITE_FAILURE
} write_ret_code;

// Write data to address.
extern write_ret_code mem_write_B (uint64_t address, char      data);
extern write_ret_code mem_write_S (uint64_t address, short     data);
extern write_ret_code mem_write_I (uint64_t address, int       data);
extern write_ret_code mem_write_L (uint64_t address, long      data);
extern write_ret_code mem_write_LL(uint64_t address, long long data);
#endif