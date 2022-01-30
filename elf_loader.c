#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>
#include "err_handler.h"
#include "mem.h"

uint64_t loadElf(const char *fileName) {
    logging(LOG_INFO, "Loading ELF executable");
    // Open the file.
    int fd = open(fileName, O_RDONLY);
    if (fd < 0) {
        perror(fileName);
        exit(-1);
    }
    
    // Get file stats.
    struct stat statBuffer;
    int rc = fstat(fd, &statBuffer);
    if (rc != 0) {
        perror("stat");
        exit(-1);
    }
    
    // Mmap the file for quick access.
    uintptr_t ptr = (uintptr_t) mmap(0, statBuffer.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if ((void *)ptr == MAP_FAILED) {
        perror("mmap");
        exit(-1);
    }
    
    // Get ELF header information.
    Elf64_Ehdr *header = (Elf64_Ehdr *) ptr;
    assert(header->e_type == ET_EXEC); // Check that it's an executable.
    uint64_t entry = header->e_entry; // Entry point of ELF executable.
    uint64_t entry_size = header->e_phentsize;
    uint64_t entry_count = header->e_phnum;
    
    // Get ELF program header and load segments.
    Elf64_Phdr *progHeader = (Elf64_Phdr *)(ptr + header->e_phoff);
    for (unsigned i = 0; i < entry_count; i++) {
        if (progHeader->p_type == PT_LOAD) {
            uint8_t *dataPtr = (uint8_t *)(ptr + progHeader->p_offset);
            uint64_t vaddr = progHeader->p_vaddr;
            uint64_t filesz = progHeader->p_filesz;
            uint64_t memsz = progHeader->p_memsz;
            // printf("%lx %lx %lx\n",vaddr, filesz, memsz);
            for (uint64_t j = 0; j < filesz; j++) {
                uint64_t addr = vaddr + j;
                uint8_t byte = dataPtr[j];
                mem_write_B(addr, byte);
            }            
        }
        progHeader = (Elf64_Phdr *) (((uintptr_t) progHeader) + entry_size);
    }

    return entry;
}
