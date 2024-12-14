#ifndef COMMON_H
#define COMMON_H

const int codelen = 1000; /* max code length in bytes */

enum cmd {
        CMD_HLT,
        CMD_PUSH,
        CMD_OUT,
        CMD_JMP
};

struct cmd_desc {
        enum cmd val;
        const char *name;
};

#endif
