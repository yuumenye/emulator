/*
 * stack - simple stack implementation modified for emulator
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stack.h"

static int stack_realloc(struct stack *stk, const int capacity);
static int stack_grow(struct stack *stk);
static int stack_shrink(struct stack *stk);
static void stack_init_to_zero(struct stack *stk);

void stack_ctor(struct stack *stk, const int capacity, const size_t elm_size)
{ 
        stk->data = calloc(capacity, sizeof(elm_size));
        if (!stk->data) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }

        stk->capacity = capacity;
        stk->elm_size = elm_size;
}

void stack_dtor(struct stack *stk)
{
        free(stk->data);
        stk->data = NULL;
}

void stack_push(struct stack *stk, void *elm)
{
        if (!stk->data)
                return;

        if (stack_grow(stk) < 0)
                return;

        memcpy((char *) stk->data + stk->size * stk->elm_size, (char *) elm,
                        stk->elm_size);

        ++stk->size;
}

static int stack_grow(struct stack *stk)
{
        if (stk->size >= stk->capacity) {
                const int grow_val = 2;
                if (stack_realloc(stk, stk->capacity * grow_val) < 0)
                        return -1;
                stack_init_to_zero(stk);
        }
        return 0;
}

void stack_pop(struct stack *stk, void *elm)
{
        if (!stk->data)
                return;

        if (stack_shrink(stk) < 0)
                return;

        --stk->size;

        memcpy((char *) elm, (char *) stk->data + stk->size * stk->elm_size,
                        stk->elm_size);

        stack_init_to_zero(stk);
}

/* print out last elm, works only for double */
void stack_peek(struct stack *stk)
{
        if (stk->size < 0)
                return;

        printf("%lg\n", *(double *) stk->data + (stk->size - 1));
}

static void stack_init_to_zero(struct stack *stk)
{
        memset((char *) stk->data + stk->size * stk->elm_size, 0,
                        (stk->capacity - stk->size) * stk->elm_size);
}

static int stack_shrink(struct stack *stk)
{
        const int shrink_lim = 4;
        if (stk->size <= stk->capacity/shrink_lim) {
                const int shrink_val = 2;
                if (stack_realloc(stk, stk->capacity/shrink_lim) < 0)
                        return -1;
                stack_init_to_zero(stk);
        }
        return 0;
}

static int stack_realloc(struct stack *stk, const int capacity)
{
        if (!stk->data)
                return -1;

        stk->data = realloc(stk->data, capacity * sizeof(stk->elm_size));
        if (!stk->data) {
                fprintf(stderr, "error: couldn't allocate memory\n");
                exit(1);
        }
        stk->capacity = capacity;
        return 0;
}
