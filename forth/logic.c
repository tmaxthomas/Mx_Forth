#include <stdint.h>

#include "logic.h"
#include "../stack.h"
#include "../sys.h"

// ( f1 f2 -- f3 )
//Bitwise and operator
void and_() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) &= s;
}

// ( f1 f2 -- f3 )
//Bitwise or operator
void or_() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) |= s;
}

// ( n1 n2 -- f )
// Compares the two numbers at the top of the stack for equality
void equals(){
    int32_t a = *(int32_t*)stack_at(0), b = *(int32_t*)stack_at(1);
    stack_pop(2);
    if(a == b)
        stack_push((int32_t) 0xffffffff);
    else
        stack_push(0x00000000);
}

// ( d1 d2 -- f )
//Compares two double-length numbers at the top of the stack for equality
void Dequals() {
    int64_t a = *(int64_t*)stack_at(0), b = *(int64_t*)stack_at(2);
    stack_pop(4);
    if(a == b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( n1 n2 -- f )
void lessThan(){
    int32_t a = *(int32_t*)stack_at(1), b = *(int32_t*)stack_at(0);
    stack_pop(2);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( u1 u2 -- f )
void UlessThan(){
    uint32_t a = *stack_at(1), b = *stack_at(0);
    stack_pop(2);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( d1 d2 -- f )
void DlessThan() {
    int64_t a = *(int64_t*)stack_at(0), b = *(int64_t*)stack_at(2);
    stack_pop(4);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( d1 d2 -- f )
void DUlessThan() {
    uint64_t a = *(uint64_t*)stack_at(1), b = *(uint64_t*)stack_at(3);
    stack_pop(4);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( n1 n2 -- f )
void greaterThan(){
    int32_t a = *(int32_t*)stack_at(1), b = *(int32_t*)stack_at(0);
    stack_pop(2);
    if(a > b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( n -- f )
// Tests whether the top of the stack is equal to zero
void zeroEquals(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(a == 0)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( d -- f )
// Tests whether the double-length top of the stack is equal to zero
void DzeroEquals(){
    int64_t a = *(int64_t*)stack_at(0);
    stack_pop(2);
    if(a == 0)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( n -- f )
// Tests whether the top of the stack is less than zero
void zeroLessThan(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(0 < a)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( n -- f )
// Tests whether the top of the stack is greater than zero
void zeroGreaterThan(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(0 > a)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
}

// ( -- f )
// Pushes true if the stack is empty, pushes false otherwise
void stack_q() {
    if(sys.stack == sys.stack_0) stack_push((int32_t)0xffffffff);
    else stack_push(0x00000000);

}
