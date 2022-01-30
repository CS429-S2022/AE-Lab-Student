#include <stdlib.h>
#include <string.h>
#include "reg.h"

static char *GPR_names[] = {
     "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",
     "8",  "9", "10", "11", "12", "13", "14", "15",
    "16", "17", "18", "19", "20", "21", "22", "23",
    "24", "25", "26", "27", "28", "29", "30"
};

static char *FPR_names[] = {
     "V0",  "V1",  "V2",  "V3",  "V4",  "V5",  "V6",  "V7",
     "V8",  "V9", "V10", "V11", "V12", "V13", "V14", "V15",
    "V16", "V17", "V18", "V19", "V20", "V21", "V22", "V23",
    "V24", "V25", "V26", "V27", "V28", "V29", "V30", "V31"
};

char *reg_prefix[] = {"W", "X"};

void init_reg(reg_t *r, char *name, unsigned index, unsigned width) {
    r->name = malloc(strlen(name)+1);
    strcpy(r->name, name);
    r->index = index;
    r->width = width;
    r->value = 0x0LL;// FIX
}

void init_reg_file(reg_file_t *rf, char *name, unsigned num, unsigned width) {
    rf->name = malloc(strlen(name)+1);
    strcpy(rf->name, name);
    rf->num = num;
    rf->width = width;
    rf->regs = malloc(num*sizeof(reg_t));
    if (0 == strcmp(name, "GPR")) {
        for (int i = 0; i < num; i++)
            init_reg(rf->regs+i, GPR_names[i], width, i);
        return;
    }
    if (0 == strcmp(name, "FPR")) {
        for (int i = 0; i < num; i++)
            init_reg(rf->regs+i, FPR_names[i], width, i);
        return;
    }
}