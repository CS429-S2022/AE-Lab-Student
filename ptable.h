#ifndef _PTABLE_H_
#define _PTABLE_H_
#include <stdint.h>

#define PAGESIZE 4096

typedef struct pte {
    uint64_t p_num;
    unsigned p_prot;
    char *p_data;
    struct pte *p_next;
} pte_t, *pte_ptr_t;

extern pte_ptr_t get_page(const uint64_t);
extern pte_ptr_t add_page(const uint64_t, const uint8_t);
#endif