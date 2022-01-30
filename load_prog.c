
#include "archsim.h"

static char scanbuf[BUF_LEN];

instr_t *load_prog(FILE *fp, unsigned long long start_addr) {
    assert(fp);
    assert((start_addr&0x3U) == 0);
    int num_instr = 0;
    unsigned long long instr_addr = start_addr;
    instr_t *ret;

    while (fscanf(fp, "%s\n", scanbuf) != EOF) {

        num_instr++;
    }
    return ret;
}