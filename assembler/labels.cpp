#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "labels.h"
#include "assembler.h"
#include "common.h"

static void move_pos(int opcode, int *ip);

void labels_ctor(struct labels *labels, int capacity)
{
        if (!labels) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        labels->data = (struct label *) calloc(capacity, sizeof(struct label));

        if (!labels->data) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }

        labels->capacity = capacity;
}

void labels_dtor(struct labels *labels)
{
        free(labels->data);
        labels->data = NULL;
}

int islabel(char *str)
{
        for (int i = 0; str[i] != '\0'; ++i) {
                if (str[i] != ':')
                        continue;
                if (str[i+1] == '\0')
                        return 1;
                break;
        }
        return 0;
}

int label_replace(char *ptr, struct labels *labels, char *arg)
{
        int val = -1;
        for (int i = 0; i < labels->size; ++i)
                if (strncmp(arg, labels->data[i].name, strlen(arg) - 1) == 0)
                        val = labels->data[i].val;
        if (val < 0)
                return val;

        memcpy(ptr, &val, sizeof(int)); 
        return val;
}

void labels_find(FILE *src, struct labels *labels)
{
        int ip = 0;
        char cmd[label_len];

        while (fscanf(src, "%s", cmd) == 1) {
                int opcode = parse_cmd(cmd); 

                if (opcode >= 0) {
                        move_pos(opcode, &ip);
                        continue;
                }
                if (islabel(cmd))
                        label_insert(labels, cmd, ip);
        }
        fseek(src, 0L, SEEK_SET);
}

static void move_pos(int opcode, int *ip)
{
        switch (opcode) {
                case CMD_HLT:
                case CMD_OUT:
                        *ip += sizeof(char);
                        break;
                case CMD_JMP:
                        *ip += sizeof(char) + sizeof(int);
                        break;
                case CMD_PUSH:
                        *ip += sizeof(char) + sizeof(double);
        }
}

void label_insert(struct labels *labels, char *name, int val)
{
        if (!labels) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        for (int i = 0; i < labels->size; ++i)
                if (strncmp(name, labels->data[i].name, strlen(name) - 1) == 0) {
                        fprintf(stderr, "error: label \"%s\" already declared\n",
                                        name);
                        exit(1);
                }

        strncpy(labels->data[labels->size].name, name, strlen(name) - 1);
        labels->data[labels->size].val = val;

        ++labels->size;
}

#ifdef DEBUG
void labels_dump(struct labels *labels)
{
        if (!labels) {
                fprintf(stderr, "error: null pointer\n");
                exit(1);
        }

        printf("labels capacity: %d\n", labels->capacity);
        printf("labels size: %d\n", labels->size);

        for (int i = 0; i < labels->size; ++i) {
                printf("name: %s\n", labels->data[i].name);
                printf("val: %d", labels->data[i].val);
                printf("\n");
        }
}
#endif
