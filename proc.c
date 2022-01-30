#include "archsim.h"

extern machine_t guest;

int runElf(const uint64_t entry) {
    logging(LOG_INFO, "Running ELF executable");
    guest.proc->PC.value = entry;
    guest.proc->SP.value = guest.mem->seg_start_addr[KERNEL_SEG]-8;
    guest.proc->NZCV.value = 0;

//  (STUDENT TODO)
    // for (int i = 0; i < 4; i++) {
    //     int instr = mem_read_I(guest.proc->PC.value);
    //     printf("%08llx: %08x\t", guest.proc->PC.value, instr);
    //     instr_t *insn = decode_instr(instr);
    //     show_instr(insn);
    //     guest.proc->PC.value += 4;
    // }
    return EXIT_SUCCESS;
}