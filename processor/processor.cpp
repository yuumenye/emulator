/*
 * processor - simple processor
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "processor.h"
#include "stack.h"

static FILE *open_bin(const char *filename);
static void read_bin(FILE *bin, struct processor *processor);
static void verify_signature(FILE *bin);
static void execute_program(struct processor *processor);
static int execute_cmd(struct processor *processor, int opcode);

static void execute_cmd_push(struct processor *processor);
static void execute_cmd_out(struct processor *processor);

#ifdef DEBUG
static void processor_dump(struct processor *processor);
#endif

void run_processor(const char *filename)
{
        struct processor processor = {};
        stack_ctor(&processor.stk, 10, sizeof(double));

        FILE *bin = open_bin(filename);
        read_bin(bin, &processor);
        fclose(bin);

        execute_program(&processor);
}

static FILE *open_bin(const char *filename)
{
        FILE *bin = fopen(filename, "rb");
        if (!bin) {
                fprintf(stderr, "error: couldn't open file\n");
                exit(1);
        }

        return bin;
}

static void read_bin(FILE *bin, struct processor *processor)
{
        if (!bin || !processor || !processor->code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        verify_signature(bin);
        fread(processor->code, sizeof(char), codelen, bin);
}

static void verify_signature(FILE *bin)
{
        char signature[] = "kya";
        int signature_len = strlen(signature);
        char tmp[signature_len] = "";

        fread(tmp, sizeof(char), signature_len, bin);
        if (strcmp(tmp, signature) != 0) {
                fprintf(stderr, "error: unknown file format\n");
                exit(1);
        }
}

static void execute_program(struct processor *processor)
{
        if (!processor || !processor->code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        while (1) {
                char opcode = processor->code[processor->ip];
                if (execute_cmd(processor, opcode) == 0)
                        break;
        }
}

static int execute_cmd(struct processor *processor, int opcode)
{
        #ifdef DEBUG
        processor_dump(processor);
        #endif

        switch (opcode) {
                case CMD_HLT:
                        return 0;
                        break;
                case CMD_PUSH:
                        execute_cmd_push(processor);
                        break;
                case CMD_OUT:
                        execute_cmd_out(processor);
                        break;
                default:
                        fprintf(stderr, "error: invalid instruction\n");
                        exit(1);
        }
        return -1;
}

static void execute_cmd_push(struct processor *processor)
{
        if (!processor || !processor->code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        ++processor->ip;

        double arg = 0;
        memcpy(&arg, processor->code + processor->ip, sizeof(double));
        stack_push(&processor->stk, &arg);

        processor->ip += sizeof(double);
}

static void execute_cmd_out(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        ++processor->ip;

        stack_peek(&processor->stk);
}

#ifdef DEBUG
static void processor_dump(struct processor *processor)
{
        int mod = 16;
        int ip = processor->ip / mod;

        for (int i = 0; i < mod; ++i)
                printf("%02X ", i);
        printf("\n\n");

        for (int i = 0; i < 16; ++i) {
                printf("%02X ", (unsigned char) processor->code[ip]);
                ++ip;
        }
        printf("\n");

        ip = processor->ip % 16;
        for (int i = 0; i < 3*ip; ++i)
                printf(" ");
        printf("^ip = %d\n\n", processor->ip);
}
#endif
