#include "../stack.h"
#include "../sys.h"

// ( n1 n2 -- n2 n1 )
// Swaps top two elements of the stack
void swap() {
    int32_t t = *stack_at(0);
    stack_pop(1);
    int32_t b = *stack_at(0);
    stack_pop(1);
    stack_push(t);
    stack_push(b);
}

// ( d1 d2 -- d2 d1 )
// Swaps top two elements of the stack for the next two
void swap2() {
    int64_t t = *(int64_t*)stack_at(0);
    stack_pop(2);
    int64_t b = *(int64_t*)stack_at(0);
    stack_pop(2);
    stack_push(t);
    stack_push(b);
}

// ( n -- n n )
// Duplicates the top of the stack
void dup() {
    stack_push(*(int32_t*)stack_at(0));
}

// ( d -- d d )
// Duplicates the top two elements of the stack
void dup2() {
    stack_push(*(int64_t*)stack_at(0));
}

//  ( f -- f f )
// Duplicates the top of the stack if it isn't 0
void dup_if() {
    int32_t q = *(int32_t*)stack_at(0);
    if(q) stack_push(q);
}

// ( n1 n2 -- n1 n2 n1 )
// Pushes the second element of the stack onto the stack
void over() {
    int32_t t = *stack_at(0);
    stack_pop(1);
    int32_t d = *stack_at(0);
    stack_push(t);
    stack_push(d);
}

// ( d1 d2 -- d1 d2 d1 )
// Pushes the third and fourth elements of the stack onto the stack
void over2() {
    int64_t t = *(int64_t*)stack_at(0);
    stack_push(t);
}

// ( n1 n2 n3 -- n2 n3 n1 )
// Removes the third element of the stack and pushes it onto the stack
void rot() {
    int32_t t = *stack_at(0);
    stack_pop(1);
    int32_t m = *stack_at(0);
    stack_pop(1);
    int32_t b = *stack_at(0);
    stack_pop(1);
    stack_push(m);
    stack_push(t);
    stack_push(b);
}

// ( n -- )
// Pops the top of the stack
void drop() {
    stack_pop(1);
}

// ( d -- )
// Pops the top 2 elements of the stack
void drop2() {
    stack_pop(2);
}

void depth() {
    stack_push(sys.stack_0 - sys.stack);
}
