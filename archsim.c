/**************************************************************************
 * C S 429 architecture emulator
 * 
 * archsim.c - The top-level loop of the emulator.
 * 
 * Copyright (c) 2022. S. Chatterjee. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "archsim.h"

machine_t guest;
FILE *infile, *outfile, *errfile;
char *ae_prompt;

int main(int argc, char* argv[]) {
    // handle_args(argc, argv);
    init();
    
    uint64_t entry = loadElf(argv[1]);
    int ret = runElf(entry);
    
    finalize();
    
    return ret;
}
