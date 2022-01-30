#include <stdlib.h>
#include "ptable.h"

#define HASHSIZE 128
static pte_ptr_t ptable[HASHSIZE];

static unsigned long ptable_hash(const uint64_t pnum) {
    unsigned long h = 0, high;
    char *s = (char *) &pnum;

    for (int i = 0; i < 7; i++) {
        h = (h << 4) + *s++;
        if (high = h & 0xF0000000)
            h ^= high >> 24;
        h &= ~high;
    }
    return h % HASHSIZE;
}

pte_ptr_t get_page(const uint64_t pnum) {
    unsigned long phash = ptable_hash(pnum);
    pte_ptr_t p = ptable[phash];
    for (; p != NULL; p = p->p_next) {
        if (pnum == p->p_num) return p;
    }
    return p;
}

pte_ptr_t add_page(const uint64_t num, const uint8_t prot) {
    pte_ptr_t npage = malloc(sizeof(pte_t));
    npage->p_num = num;
    npage->p_prot = prot;
    npage->p_data = calloc(PAGESIZE,sizeof(char));
    unsigned long phash = ptable_hash(num);
    npage->p_next = ptable[phash];
    ptable[phash] = npage;
    return npage;
}