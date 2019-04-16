#include <stdint.h>

#include "logic.h"
#include "../stack.h"
#include "../sys.h"

// ( f1 f2 -- f3 )
// Bitwise and operator
void and_() {
    int32_t s = *(int32_t *) stack_at(0);
    stack_pop(1);
    *(int32_t *) stack_at(0) &= s;
}

// ( f1 f2 -- f3 )
// Bitwise or operator
void or_() {
    int32_t s = *(int32_t *) stack_at(0);
    stack_pop(1);
    *(int32_t *) stack_at(0) |= s;
}

// ( n1 n2 -- f )
// Compares the two numbers at the top of the stack for equality
void equals(){
    int32_t a = *(int32_t *) stack_at(0),
            b = *(int32_t*)stack_at(1);
    stack_pop(2);
    stack_push(a == b ? -1 : 0);
}

// ( n1 n2 -- f )
void lessThan(){
    int32_t a = *(int32_t *) stack_at(1),
            b = *(int32_t*)stack_at(0);
    stack_pop(2);
    stack_push(a < b ? -1 : 0);
}

// ( u1 u2 -- f )
void UlessThan(){
    uint32_t a = *stack_at(1),
             b = *stack_at(0);
    stack_pop(2);
    stack_push(a < b ? -1 : 0);
}

// ( n1 n2 -- f )
void greaterThan(){
    int32_t a = *(int32_t *) stack_at(1),
            b = *(int32_t*)stack_at(0);
    stack_pop(2);
    stack_push(a > b ? -1 : 0);
}

// ( n -- f )
// Tests whether the top of the stack is equal to zero
void zeroEquals(){
    int32_t a = *(int32_t *) stack_at(0);
    stack_pop(1);
    stack_push(a == 0 ? -1 : 0);
}

// ( n -- f )
// Tests whether the top of the stack is less than zero
void zeroLessThan(){
    int32_t a = *(int32_t *) stack_at(0);
    stack_pop(1);
    stack_push(0 < a ? -1 : 0);
}
