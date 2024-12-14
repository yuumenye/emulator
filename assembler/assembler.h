#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "common.h"

struct code {
        char code[codelen];
        char *ptr;
};

void run_assembler(const char *filename);
int parse_cmd(const char *cmd);

#endif
