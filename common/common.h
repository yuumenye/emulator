#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

const int codelen = 1000; /* max code length in bytes */

enum cmd {
        CMD_HLT,
        CMD_PUSH,
        CMD_ADD,
        CMD_SUB,
        CMD_MUL,
        CMD_DIV,
        CMD_OUT,
        CMD_IN,
        CMD_SQRT,
        CMD_SIN,
        CMD_COS,
        CMD_JMP,
        CMD_JA,
        CMD_JAE,
        CMD_JB,
        CMD_JBE,
        CMD_JE,
        CMD_JNE
};

struct cmd_desc {
        enum cmd val;
        const char *name;
};

struct header {
        uint32_t signature;
        uint32_t v;
        uint32_t size;
        uint32_t reserved;
};

#endif
