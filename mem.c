#include <stdio.h>
#include <stdint.h>
#include "err_handler.h"
#include "mem.h"
#include "ptable.h"
#include "machine.h"

extern machine_t guest;

static byte_order_t get_byte_order(const uint64_t addr) {
    if ((guest.mem->seg_start_addr[TEXT_SEG] <= addr) && 
        (addr < guest.mem->seg_start_addr[DATA_SEG]))
        return guest.code_order;
    return guest.data_order;
}

static uint8_t get_prot_bits(const uint64_t addr) {
    for (int i = 0; i < KERNEL_SEG; i++) {
        if ((guest.mem->seg_start_addr[i] <= addr) && 
            (addr < guest.mem->seg_start_addr[i+1]))
            return guest.mem->seg_prot[i];
    }
    return guest.mem->seg_prot[KERNEL_SEG];
}

static uint8_t _mem_read_byte(const uint64_t addr) {
    uint64_t pnum = addr / PAGESIZE;
    uint64_t poff = addr % PAGESIZE;
    pte_ptr_t page = get_page(pnum);
    if (NULL == page)
        page = add_page(pnum, get_prot_bits(addr));
    return page->p_data[poff];
}

static uint64_t _mem_read_LE(const uint64_t addr, const unsigned width) {
    uint64_t retval = 0ULL;
    for (int i = width-1; i >= 0; i--)
        retval = (retval << 8) + _mem_read_byte(addr+i);
    return retval;
}

static uint64_t _mem_read_BE(const uint64_t addr, const unsigned width) {
    uint64_t retval = 0ULL;
    for (int i = 0; i < width; i++)
        retval = (retval << 8) + _mem_read_byte(addr+i);
    return retval;
}

uint64_t _mem_read(const uint64_t addr, const unsigned width) {
    byte_order_t b = get_byte_order(addr);
    switch (b) {
        case L_ENDIAN:
            return _mem_read_LE(addr, width);
        case B_ENDIAN:
            return _mem_read_BE(addr, width);
        default:
            return 0;// FIX
    }
}

static write_ret_code _mem_write_byte(const uint64_t addr, const uint8_t data) {
    uint64_t pnum = addr / PAGESIZE;
    uint64_t poff = addr % PAGESIZE;
    pte_ptr_t page = get_page(pnum);
    if (NULL == page) {
        page = add_page(pnum, 7);//FIX.
    }
    page->p_data[poff] = data;
    //printf("%lx:%lx: %x\n", pnum, poff, data);
    return WRITE_SUCCESS;
}

static write_ret_code _mem_write_LE(const uint64_t addr, const uint64_t data, const unsigned width) {
    uint8_t *s = (uint8_t *) &data;
    write_ret_code retval = WRITE_FAILURE;
    for (int i = 0; i < width; i++)
        retval |= _mem_write_byte(addr+i, s[width-i-1]);
    return retval;
}

static write_ret_code _mem_write_BE(const uint64_t addr, const uint64_t data, const unsigned width) {
    uint8_t *s = (uint8_t *) &data;
    write_ret_code retval = WRITE_FAILURE;
    for (int i = 0; i < width; i++)
        retval |= _mem_write_byte(addr+i, s[i]);
    return retval;
}


write_ret_code _mem_write(const uint64_t addr, const uint64_t data, const unsigned width) {
    byte_order_t b = get_byte_order(addr);
    switch (b) {
        case L_ENDIAN:
            return _mem_write_LE(addr, data, width);
        case B_ENDIAN:
            return _mem_write_BE(addr, data, width);
        default:
            return WRITE_FAILURE;
    }
}

char      mem_read_B (const uint64_t addr) {return (char)      _mem_read(addr, 1);}
short     mem_read_S (const uint64_t addr) {return (short)     _mem_read(addr, 2);}
int       mem_read_I (const uint64_t addr) {return (int)       _mem_read(addr, 4);}
long      mem_read_L (const uint64_t addr) {return (long)      _mem_read(addr, 8);}
long long mem_read_LL(const uint64_t addr) {return (long long) _mem_read(addr, 8);}

write_ret_code mem_write_B (const uint64_t addr, const char      data) {return _mem_write(addr, (uint64_t) data, 1);}
write_ret_code mem_write_S (const uint64_t addr, const short     data) {return _mem_write(addr, (uint64_t) data, 2);}
write_ret_code mem_write_I (const uint64_t addr, const int       data) {return _mem_write(addr, (uint64_t) data, 4);}
write_ret_code mem_write_L (const uint64_t addr, const long      data) {return _mem_write(addr, (uint64_t) data, 8);}
write_ret_code mem_write_LL(const uint64_t addr, const long long data) {return _mem_write(addr, (uint64_t) data, 8);}