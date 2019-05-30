#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "intmath.h"
#include "../stack.h"
#include "../sys.h"

// ( n1 n2 -- sum )
// Adds n2 to n1
void add() {
    int32_t s = *(int32_t *) stack_at(0);
    stack_pop(1);
    *(int32_t *) stack_at(0) += s;
}

// ( n1 n2 -- diff )
// Subtracts n2 from n1
void sub() {
    int32_t s = *(int32_t *) stack_at(0);
    stack_pop(1);
    *(int32_t *) stack_at(0) -= s;
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2
void mult() {
    int32_t s = *(int32_t *) stack_at(0);
    stack_pop(1);
    *(int32_t *) stack_at(0) *= s;
}

// ( u1 u2 -- prod )
// Multiplies u1 by u2
void umult() {
    int32_t s = *stack_at(0),
            m = *stack_at(1);
    stack_pop(2);
    stack_push_d((int64_t)s*m);
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2 - double length result
void Mmult() {
    int32_t s = *(int32_t *) stack_at(0),
            m = *(int32_t *) stack_at(1);
    stack_pop(2);
    stack_push_d((int64_t)s*m);
}

// ( n1 n2 -- qout )
// Divides n1 by n2
void divd() {
    int32_t s = *(int32_t *) stack_at(0);
    stack_pop(1);
    *(int32_t *) stack_at(0) /= s;
}

// ( n1 n2 -- rem )
// Computes n1 mod n2
void mod() {
    int32_t s = *(int32_t *) stack_at(0);
    stack_pop(1);
    *(int32_t *) stack_at(0) %= s;
}

// ( n1 n2 -- rem quot )
// Divides n1 by n2, giving remainder and quotient
void modDiv() {
    int32_t m, s = *(int32_t *) stack_at(0);
    stack_pop(1);
    m = *(int32_t *) stack_at(0) / s;
    *(int32_t *) stack_at(0) %= s;
    stack_push(m);
}

// ( u1 u2 -- quot )
// Divides u1 by u2
void UmodDiv() {
    uint32_t m, s = *stack_at(0);
    stack_pop(1);
    m = *stack_at(0) % s;
    *stack_at(0) /= s;
    // Pointer hackery to get an unsigned int32_t pushed onto the stack
    stack_push(*(int32_t *) &m);
}

// ( d n1  -- n2 n3 )
// Divides d by n1, giving symmetric quotient n3 and remainder n2
void SmodDiv() {
    int32_t n = *(int32_t *) stack_at(0);
    int64_t s = *(int64_t *) stack_at(1);
    stack_pop(3);
    stack_push((int32_t) s % n);
    stack_push((int32_t) round((float) s / n ));
}

// ( d n1 -- n2 n3 )
// Divides d by n1, giving floored quotient n3 and remainder n2
void FmodDiv() {
    int32_t n = *(int32_t *) stack_at(0);
    int64_t s = *(int64_t *) stack_at(1);
    stack_pop(3);
    stack_push((int32_t) s % n);
    stack_push((int32_t) s / n);
}

// ( n1 n2 n3 -- n-result )
// Multiplies n1 by n2, then divides by n3. Uses a double length intermediate.
void multDiv(){
    int64_t a = *(int32_t *) stack_at(2),
            b = *(int32_t *) stack_at(1),
            c = *(int32_t *) stack_at(0);
    stack_pop(3);
    int64_t m = a * b;
    int32_t d = (int32_t)(m / c);
    stack_push(d);
}

// ( n1 n2 n3 -- n-rem, n-result)
// Multiplies n1 by n2, then divides by n3. Returns the quotient and remainder. Uses a double-length intermediate.
void multDivMod(){
    long a = *(int32_t *) stack_at(2),
         b = *(int32_t *) stack_at(1),
         c = *(int32_t *) stack_at(0);
    stack_pop(3);
    long m = a * b;
    int32_t d = (int32_t)(m / c), r = (int32_t)(m % c);
    stack_push(r);
    stack_push(d);
}

// ( n1 -- n2 )
// Increments the top of the stack
void add1() {
    (*(int32_t *) stack_at(0))++;
}

// ( n1 -- n2 )
// Decrements the top of the stack
void sub1() {
    (*(int32_t *) stack_at(0))--;
}

// ( n1 -- n2 )
// Leftshifts the top of the stack by 1
void lshift() {
    *(int32_t *) stack_at(0) *= 2;
}

// ( n1 -- n2 )
// Rightshifts the top of the stack by 1
void rshift() {
    *(int32_t *) stack_at(0) /= 2;
}


// ( n -- u)
// Computes absolute value of the top of the stack
void abs_(){
    *(int32_t *) stack_at(0) = abs(*(int32_t *) stack_at(0));
}

// ( n1 -- n2 )
// Negates the top of the stack
void neg(){
    *(int32_t *) stack_at(0) *= -1;
}

// ( n1 n2 -- n3 )
// Returns minimum of n1 and n2
void min(){
    int32_t a = *(int32_t *) stack_at(1),
            b = *(int32_t *) stack_at(0);
    stack_pop(2);
    stack_push(a < b ? a : b);
}

// ( n1 n2 -- n3 )
// Returns maximum of 2 numbers
void max(){
    int32_t a = *(int32_t *) stack_at(1),
            b = *(int32_t *) stack_at(0);
    stack_pop(2);
    stack_push(a > b ? a : b);
}

void base() {
    stack_push(forth_addr(&sys.base));
}

void decimal() {
    sys.base = 10;
}

void invert() {
    uint32_t *a = stack_at(0);
    *a = ~*a;
}

void xor() {
    uint32_t a = *stack_at(0),
             b = *stack_at(1);
    stack_pop(2);
    stack_push(a ^ b);
}

void stod() {
    int32_t num = *stack_at(0);
    stack_pop(1);
    stack_push(num);
}
