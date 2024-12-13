#ifndef STACK_H
#define STACK_H

struct stack {
        void *data;
        size_t elm_size;
        int capacity;
        int size;
};

void stack_ctor(struct stack *stk, const int capacity, const size_t elm_size);
void stack_dtor(struct stack *stk);

void stack_push(struct stack *stk, void *elm);
void stack_pop(struct stack *stk, void *elm);
void stack_peek(struct stack *stk);

#endif
