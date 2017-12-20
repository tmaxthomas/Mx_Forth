#include <stdint.h>

#include "logic.h"
#include "../stack.h"

// ( f1 f2 -- f3 )
//Binary and operator
int32_t and_() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) &= s;
    return 0;
}

// ( f1 f2 -- f3 )
//Binary or operator
int32_t or_() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) |= s;
    return 0;
}

// ( n1 n2 -- f )
// Compares the two numbers at the top of the stack for equality
int32_t equals(){
    int32_t a = *(int32_t*)stack_at(0), b = *(int32_t*)stack_at(1);
    stack_pop(2);
    if(a == b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
//Compares two double-length numbers at the top of the stack for equality
int32_t Dequals() {
    int64_t a = *(int64_t*)stack_at(0), b = *(int64_t*)stack_at(2);
    stack_pop(4);
    if(a == b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n1 n2 -- f )
int32_t lessThan(){
    int32_t a = *(int32_t*)stack_at(1), b = *(int32_t*)stack_at(0);
    stack_pop(2);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( u1 u2 -- f )
int32_t UlessThan(){
    uint32_t a = *stack_at(1), b = *stack_at(0);
    stack_pop(2);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
int32_t DlessThan() {
    int64_t a = *(int64_t*)stack_at(0), b = *(int64_t*)stack_at(2);
    stack_pop(4);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
int32_t DUlessThan() {
    uint64_t a = *(uint64_t*)stack_at(1), b = *(uint64_t*)stack_at(3);
    stack_pop(4);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n1 n2 -- f )
int32_t greaterThan(){
    int32_t a = *(int32_t*)stack_at(1), b = *(int32_t*)stack_at(0);
    stack_pop(2);
    if(a > b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is equal to zero
int32_t zeroEquals(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(a == 0)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d -- f )
// Tests whether the double-length top of the stack is equal to zero
int32_t DzeroEquals(){
    int64_t a = *(int64_t*)stack_at(0);
    stack_pop(2);
    if(a == 0)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is less than zero
int32_t zeroLessThan(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(0 < a)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is greater than zero
int32_t zeroGreaterThan(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(0 > a)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}
