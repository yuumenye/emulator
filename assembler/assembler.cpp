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
static void insert_cmd(FILE *src, struct code *code, char opcode);
static FILE *create_bin(void);
static void write_bin(FILE *bin, struct code *code);

static void insert_cmd_no_arg(struct code *code, char opcode);
static void insert_cmd_push(FILE *src, struct code *code, char opcode);
static void insert_cmd_jmp(FILE *src, struct code *code, char opcode);

static struct cmd_desc cmds[] = {CMD_HLT,  "hlt",
                CMD_PUSH, "push", CMD_OUT,  "out", CMD_JMP, "jmp"};

static const size_t ncmds = sizeof(cmds)/sizeof(cmds[0]);

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
                        fprintf(stderr, "error: invalid command \"%s\"\n", cmd);
                        exit(1);
                }
                insert_cmd(src, code, opcode);
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

static void insert_cmd(FILE *src, struct code *code, char opcode)
{
        switch (opcode) {
                case CMD_HLT:
                case CMD_OUT:
                        insert_cmd_no_arg(code, opcode);
                        break;
                case CMD_PUSH:
                        insert_cmd_push(src, code, opcode);
                        break;
                case CMD_JMP:
                        insert_cmd_jmp(src, code, opcode);
                        break;
                default:
                        fprintf(stderr, "error: invalid command");
                        exit(1);
        }
}

static void insert_cmd_no_arg(struct code *code, char opcode)
{
        if (!code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        memcpy(code->ptr, &opcode, sizeof(char));
        code->ptr += sizeof(char);
}

static void insert_cmd_push(FILE *src, struct code *code, char opcode)
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

static void insert_cmd_jmp(FILE *src, struct code *code, char opcode)
{
        if (!code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        memcpy(code->ptr, &opcode, sizeof(char));
        code->ptr += sizeof(char);

        int arg = 0;
        fscanf(src, "%d", &arg);
        memcpy(code->ptr, &arg, sizeof(int));
        code->ptr += sizeof(int);
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
