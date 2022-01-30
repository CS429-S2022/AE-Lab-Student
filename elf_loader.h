#ifndef _ELF_LOADER_H_
#define _ELF_LOADER_H_
#include <stdint.h>

extern uint64_t loadElf(const char *file);
#endif