/*
 * assembler - convert source file into binary
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "common.h"
#include "assembler.h"
#include "labels.h"

static FILE *open_src(const char *filename, struct code *code);
static void translate_src(FILE *src, struct code *code);
int parse_cmd(const char *cmd);
static void insert_cmd(FILE *src, struct code *code,
                struct labels *labels, char opcode);
static FILE *create_bin(void);
static void write_bin(FILE *bin, struct code *code);

static void insert_cmd_no_arg(struct code *code, char opcode);
static void insert_cmd_push(FILE *src, struct code *code, char opcode);
static void insert_cmd_jmp(FILE *src, struct code *code,
                struct labels *labels, char opcode);

static struct cmd_desc cmds[] = {CMD_HLT, "hlt", CMD_PUSH, "push",
        CMD_ADD, "add", CMD_SUB, "sub", CMD_MUL, "mul", CMD_DIV, "div",
        CMD_OUT, "out", CMD_IN, "in", CMD_SQRT, "sqrt", CMD_SIN, "sin",
        CMD_COS, "cos", CMD_JMP, "jmp", CMD_JA, "ja", CMD_JAE, "jae",
        CMD_JB, "jb", CMD_JBE, "jbe", CMD_JE, "je", CMD_JNE, "jne"};

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

        struct labels labels = {};
        labels_ctor(&labels, 10);
        labels_find(src, &labels);
        labels_dump(&labels);

        while (fscanf(src, "%s", cmd) == 1) {
                char opcode = 0;
                if ((opcode = parse_cmd(cmd)) >= 0) {
                        insert_cmd(src, code, &labels, opcode);
                        continue;
                }
                if (islabel(cmd))
                        continue;
                fprintf(stderr, "error: invalid command \"%s\"\n", cmd);
                exit(1);
        }
}

int parse_cmd(const char *cmd)
{
        for (int i = 0; i < ncmds; ++i) {
                if (strcmp(cmd, cmds[i].name) == 0)
                        return cmds[i].val;
        }
        return -1;
}

static void insert_cmd(FILE *src, struct code *code, struct labels *labels,
                char opcode)
{
        switch (opcode) {
                case CMD_HLT:
                case CMD_ADD:
                case CMD_SUB:
                case CMD_MUL:
                case CMD_DIV:
                case CMD_OUT:
                case CMD_IN:
                case CMD_SQRT:
                case CMD_SIN:
                case CMD_COS:
                        insert_cmd_no_arg(code, opcode);
                        break;
                case CMD_PUSH:
                        insert_cmd_push(src, code, opcode);
                        break;
                case CMD_JMP:
                case CMD_JA:
                case CMD_JAE:
                case CMD_JB:
                case CMD_JBE:
                case CMD_JE:
                case CMD_JNE:
                        insert_cmd_jmp(src, code, labels, opcode);
                        break;
                default:
                        fprintf(stderr, "error: invalid command\n");
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

static void insert_cmd_jmp(FILE *src, struct code *code, struct labels *labels, char opcode)
{
        if (!code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        memcpy(code->ptr, &opcode, sizeof(char));
        code->ptr += sizeof(char);

        char arg[label_len] = "";
        fscanf(src, "%s", arg);

        if (label_replace(code->ptr, labels, arg) < 0) {
                int pos = atoi(arg);
                memcpy(code->ptr, &arg, sizeof(int));
        }

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

        uint32_t code_size = code->ptr - code->code;

        struct header header = {0x61796b, 1, code_size, 0};

        fwrite(&header, 1, sizeof(struct header), bin);

        fwrite(code, code_size, 1, bin); 
}
