/*
 * processor - simple processor
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "common.h"
#include "processor.h"
#include "stack.h"

static FILE *open_bin(const char *filename);
static void read_bin(FILE *bin, struct processor *processor);
static void verify_signature(FILE *bin);
static void execute_program(struct processor *processor);
static int execute_cmd(struct processor *processor, int opcode);

static void execute_cmd_push(struct processor *processor);
static void execute_cmd_add(struct processor *processor);
static void execute_cmd_sub(struct processor *processor);
static void execute_cmd_mul(struct processor *processor);
static void execute_cmd_div(struct processor *processor);
static void execute_cmd_out(struct processor *processor);
static void execute_cmd_in(struct processor *processor);
static void execute_cmd_sqrt(struct processor *processor);
static void execute_cmd_sin(struct processor *processor);
static void execute_cmd_cos(struct processor *processor);

static void execute_cmd_jmp(struct processor *processor);
static void execute_cmd_ja(struct processor *processor);
static void execute_cmd_jae(struct processor *processor);
static void execute_cmd_jb(struct processor *processor);
static void execute_cmd_jbe(struct processor *processor);
static void execute_cmd_je(struct processor *processor);
static void execute_cmd_jne(struct processor *processor);

#ifdef DEBUG
static void processor_dump(struct processor *processor);
#endif

#define BIN_ARITHMETIC(OP) double a = 0, b = 0, c = 0;      \
                           ++processor->ip;                 \
                           stack_pop(&processor->stk, &a);  \
                           stack_pop(&processor->stk, &b);  \
                           c = b OP a;                      \
                           stack_push(&processor->stk, &c);

#define UNARY_ARITHMETIC(OP) double a = 0, b = 0;            \
                             ++processor->ip;                \
                             stack_pop(&processor->stk, &a); \
                             b = OP(a);                      \
                             stack_push(&processor->stk, &b);

#define CONDITIONAL_JMP(OP) int arg = 0;                                  \
                            double a = 0, b = 0;                          \
                            ++processor->ip;                              \
                            memcpy(&arg, processor->code + processor->ip, \
                                            sizeof(int));                 \
                            stack_pop(&processor->stk, &a);               \
                            stack_pop(&processor->stk, &b);               \
                            if (b OP a)                                   \
                                    processor->ip = arg;

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
                case CMD_ADD:
                        execute_cmd_add(processor);
                        break;
                case CMD_SUB:
                        execute_cmd_sub(processor);
                        break;
                case CMD_MUL:
                        execute_cmd_mul(processor);
                        break;
                case CMD_DIV:
                        execute_cmd_div(processor);
                        break;
                case CMD_OUT:
                        execute_cmd_out(processor);
                        break;
                case CMD_IN:
                        execute_cmd_in(processor);
                        break;
                case CMD_SQRT:
                        execute_cmd_sqrt(processor);
                        break;
                case CMD_SIN:
                        execute_cmd_sin(processor);
                        break;
                case CMD_COS:
                        execute_cmd_cos(processor);
                        break;
                case CMD_JMP:
                        execute_cmd_jmp(processor);
                        break;
                case CMD_JA:
                        execute_cmd_ja(processor);
                        break;
                case CMD_JAE:
                        execute_cmd_jae(processor);
                        break;
                case CMD_JB:
                        execute_cmd_jb(processor);
                        break;
                case CMD_JBE:
                        execute_cmd_jbe(processor);
                        break;
                case CMD_JE:
                        execute_cmd_je(processor);
                        break;
                case CMD_JNE:
                        execute_cmd_jne(processor);
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

static void execute_cmd_add(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        BIN_ARITHMETIC(+);
}

static void execute_cmd_sub(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        BIN_ARITHMETIC(-);
}

static void execute_cmd_mul(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        BIN_ARITHMETIC(*);
}

static void execute_cmd_div(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        BIN_ARITHMETIC(/);
}

static void execute_cmd_in(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        double arg = 0;
        scanf("%lg", &arg);
        stack_push(&processor->stk, &arg);
}

static void execute_cmd_sqrt(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        UNARY_ARITHMETIC(sqrt);
}

static void execute_cmd_sin(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        UNARY_ARITHMETIC(sin);
}

static void execute_cmd_cos(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        UNARY_ARITHMETIC(cos);
}

static void execute_cmd_jmp(struct processor *processor)
{
        if (!processor || !processor->code) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        ++processor->ip;

        int arg = 0;
        memcpy(&arg, processor->code + processor->ip, sizeof(int));
        processor->ip = arg;
}

static void execute_cmd_ja(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        CONDITIONAL_JMP(>);
}

static void execute_cmd_jae(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        CONDITIONAL_JMP(>=);
}

static void execute_cmd_jb(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        CONDITIONAL_JMP(<);
}

static void execute_cmd_jbe(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        CONDITIONAL_JMP(<=);
}

static void execute_cmd_je(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        CONDITIONAL_JMP(==);
}

static void execute_cmd_jne(struct processor *processor)
{
        if (!processor) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        CONDITIONAL_JMP(!=);
}

#ifdef DEBUG
static void processor_dump(struct processor *processor)
{
        int mod = 16;

        printf("-----------------------------------------------\n");
        for (int i = 0; i < mod; ++i)
                printf("%02X ", i);
        printf("\n\n");

        for (int i = 0; i < mod; ++i)
                printf("%02X ", (unsigned char) processor->code[i +
                                (processor->ip / mod) * mod]);
        printf("\n");

        int remainder = processor->ip % mod;
        for (int i = 0; i < 3*remainder; ++i)
                printf(" ");
        printf("^ip = %d\n\n", processor->ip);
        printf("-----------------------------------------------\n");
}
#endif
