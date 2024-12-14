#ifndef LABELS_H
#define LABELS_H

const int label_len = 15;

struct label {
        int val;
        char name[label_len];
};

struct labels {
        struct label *data;
        int size;
        int capacity;
};

void labels_ctor(struct labels *labels, int capacity);
void labels_dtor(struct labels *labels);

int islabel(char *str);
void label_insert(struct labels *labels, char *name, int val);
int label_replace(char *ptr, struct labels *labels, char *arg);
void labels_find(FILE *src, struct labels *labels);
#ifdef DEBUG
void labels_dump(struct labels *labels);
#endif

#endif
