#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "common.h"
#include "stack.h"

struct processor {
        char code[codelen];
        int ip;
        struct stack stk;
};

void run_processor(const char *filename);

#endif
