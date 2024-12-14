/*
 * assembler - convert source file into binary
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "assembler.h"

static FILE *open_src(const char *filename, struct code *code);
static void translate_src(FILE *src, struct code *code);
static int parse_cmd(const char *cmd);
static void append_cmd(FILE *src, struct code *code, char opcode);
static FILE *create_bin(void);
static void write_bin(FILE *bin, struct code *code);

static int cmd_hlt_cont(FILE *src, struct code *code, char opcode);
static int cmd_push_cont(FILE *src, struct code *code, char opcode);
static int cmd_out_cont(FILE *src, struct code *code, char opcode);

static void append_cmd_no_arg(struct code *code, char opcode);
static void append_cmd_push(FILE *src, struct code *code, char opcode);

static struct cmd_desc cmds[] = {CMD_HLT,  "hlt",
                CMD_PUSH, "push", CMD_OUT,  "out"};

static const size_t ncmds = sizeof(cmds)/sizeof(cmds[0]);

static int (*cmd_funcs[])(FILE *src, struct code *code, char opcode) =
{cmd_hlt_cont, cmd_push_cont, cmd_out_cont};

static const size_t n_cmd_funcs = sizeof(cmd_funcs)/sizeof(cmd_funcs[0]);

void run_assembler(const char *filename)
{
        struct code code = {};
        code.ptr = code.code;

        FILE *src = open_src(filename, &code);
        translate_src(src, &code);
        fclose(src);

        FILE *bin = create_bin();
        write_bin(bin, &code);
        fclose(bin);
}

static FILE *open_src(const char *filename, struct code *code)
{
        FILE *src = fopen(filename, "r");
        if (!src) {
                fprintf(stderr, "error: couldn't open file\n");
                exit(1);
        }
        return src;
}

static void translate_src(FILE *src, struct code *code)
{
        const int cmdlen = 10;
        char cmd[cmdlen] = {};

        while (fscanf(src, "%s", cmd) == 1) {
                char opcode = 0;
                if ((opcode = parse_cmd(cmd)) < 0) {
                        fprintf(stderr, "error: invalid command\n");
                        exit(1);
                }
                append_cmd(src, code, opcode);
        }
}

static int parse_cmd(const char *cmd)
{
        for (int i = 0; i < ncmds; ++i) {
                if (strcmp(cmd, cmds[i].name) == 0)
                        return cmds[i].val;
        }
        return -1;
}

static void append_cmd(FILE *src, struct code *code, char opcode)
{
        for (int i = 0; i < n_cmd_funcs; ++i)
                if (cmd_funcs[i](src, code, opcode) < 0)
                        continue;
}

static int cmd_hlt_cont(FILE *src, struct code *code, char opcode)
{
        if (opcode != CMD_HLT)
                return -1;

        append_cmd_no_arg(code, opcode);
        return 0;
}

static int cmd_push_cont(FILE *src, struct code *code, char opcode)
{
        if (opcode != CMD_PUSH)
                return -1;
        
        append_cmd_push(src, code, opcode);
        return 0;
}

static int cmd_out_cont(FILE *src, struct code *code, char opcode)
{
        if (opcode != CMD_OUT)
                return -1;

        append_cmd_no_arg(code, opcode);
        return 0;
}

static void append_cmd_no_arg(struct code *code, char opcode)
{
        if (!code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        memcpy(code->ptr, &opcode, sizeof(char));
        code->ptr += sizeof(char);
}

static void append_cmd_push(FILE *src, struct code *code, char opcode)
{
        if (!code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        memcpy(code->ptr, &opcode, sizeof(char));
        code->ptr += sizeof(char);

        double arg = 0;
        fscanf(src, "%lg", &arg);
        memcpy(code->ptr, &arg, sizeof(double));
        code->ptr += sizeof(double);
}

static FILE *create_bin(void)
{
        FILE *bin = fopen("program", "wb+");
        if (!bin) {
                fprintf(stderr, "error: couldn't open file\n");
                exit(1);
        }

        return bin;
}

static void write_bin(FILE *bin, struct code *code)
{
        if (!bin) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        char signature[] = "kya";
        int signature_len = strlen(signature);

        fwrite(signature, sizeof(char), signature_len, bin);

        int code_size = code->ptr - code->code;
        fwrite(code, code_size, 1, bin); 
}
