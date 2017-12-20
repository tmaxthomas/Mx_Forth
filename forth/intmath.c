#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "intmath.h"
#include "../stack.h"

// ( n1 n2 -- sum )
//Adds n2 to n1
int32_t add() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) += s;
    return 0;
}

// ( d1 d2 -- sum )
// Adds d2 to d1
int32_t Dadd() {
    int64_t s = *(int64_t*)stack_at(0);
    stack_pop(2);
    *(int64_t*)stack_at(0) += s;
    return 0;
}

// ( d n -- sum )
// Adds n to d - double length result
int32_t Madd() {
    int32_t n = *(int32_t*)stack_at(0);
    stack_pop(2);
    *(int64_t*)stack_at(0) += (int64_t)n;
    return 0;
}

// ( n1 n2 -- diff )
// Subtracts n2 from n1
int32_t sub() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) -= s;
    return 0;
}

// ( d1 d2 -- diff )
// Subtracts d2 from d1
int32_t Dsub() {
    int64_t s = *(int64_t*)stack_at(0);
    stack_pop(2);
    *(int64_t*)stack_at(0) -= s;
    return 0;
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2
int32_t mult() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) *= s;
    return 0;
}

// ( u1 u2 -- prod )
// Multiplies u1 by u2
int32_t umult() {
    int32_t s = *stack_at(0);
    int32_t m = *stack_at(1);
    stack_pop(2);
    stack_push((int64_t)s*m);
    return 0;
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2 - double length result
int32_t Mmult() {
    int32_t s = *(int32_t*)stack_at(0);
    int32_t m = *(int32_t*)stack_at(1);
    stack_pop(2);
    stack_push((int64_t)s*m);
    return 0;
}

// ( n1 n2 -- qout )
// Divides n1 by n2
int32_t div() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) /= s;
    return 0;
}

// ( n1 n2 -- rem )
// Computes n1 mod n2
int32_t mod() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) %= s;
    return 0;
}

// ( n1 n2 -- rem quot )
// Divides n1 by n2, giving remainder and quotient
int32_t modDiv() {
    int32_t m, s = *(int32_t*)stack_at(0);
    stack_pop(1);
    m = *(int32_t*)stack_at(0) / s;
    *(int32_t*)stack_at(0) %= s;
    stack_push(m);
    return 0;
}

// ( u1 u2 -- quot )
// Divides u1 by u2
int32_t UmodDiv() {
    uint32_t m, s = *stack_at(0);
    stack_pop(1);
    m = *stack_at(0) % s;
    *stack_at(0) /= s;
    //Pointer hackery to get an unsigned int32_t pushed onto the stack
    stack_push(*(int32_t*)&m);
    return 0;
}

// ( d n1  -- n2 n3 )
// Divides d by n1, giving symmetric quotient n3 and remainder n2
int32_t SmodDiv() {
    int32_t n = *(int32_t*)stack_at(0);
    int64_t s = *(int64_t*)stack_at(1);
    stack_pop(3);
    stack_push((int32_t) s % n);
    stack_push((int32_t) round((float) s / n ));
    return 0;
}

// ( d n1 -- n2 n3 )
// Divides d by n1, giving floored quotient n3 and remainder n2
int32_t FmodDiv() {
    int32_t n = *(int32_t*)stack_at(0);
    int64_t s = *(int64_t*)stack_at(1);
    stack_pop(3);
    stack_push((int32_t) s % n);
    stack_push((int32_t) s / n);
    return 0;
}

// ( n1 n2 n3 -- n-result )
// Multiplies n1 by n2, then divides by n3. Uses a double length intermediate.
int32_t multDiv(){
    int64_t a = *(int32_t*)stack_at(2), b = *(int32_t*)stack_at(1), c = *(int32_t*)stack_at(0);
    stack_pop(3);
    int64_t m = a * b;
    int32_t d = (int32_t)(m / c);
    stack_push(d);
    return 0;
}

// ( n1 n2 n3 -- n-rem, n-result)
// Multiplies n1 by n2, then divides by n3. Returns the quotient and remainder. Uses a double-length intermediate.
int32_t multDivMod(){
    long a = *(int32_t*)stack_at(2), b = *(int32_t*)stack_at(1), c = *(int32_t*)stack_at(0);
    stack_pop(3);
    long m = a * b;
    int32_t d = (int32_t)(m / c), r = (int32_t)(m % c);
    stack_push(r);
    stack_push(d);
    return 0;
}

// ( n1 -- n2 )
//Increments the top of the stack
int32_t add1() {
    (*(int32_t*)stack_at(0))++;
    return 0;
}

// ( n1 -- n2 )
//Decrements the top of the stack
int32_t sub1() {
    (*(int32_t*)stack_at(0))--;
    return 0;
}

// ( n1 -- n2 )
//Adds 2 to the top of the stack
int32_t add2() {
    *(int32_t*)stack_at(0) += 2;
    return 0;
}

// ( n1 -- n2 )
//Subtracts 2 from the top of the stack
int32_t sub2() {
    *(int32_t*)stack_at(0) -= 2;
    return 0;
}

// ( n1 -- n2 )
//Leftshifts the top of the stack by 1
int32_t lshift() {
    *(int32_t*)stack_at(0) *= 2;
    return 0;
}

// ( n1 -- n2 )
//Rightshifts the top of the stack by 1
int32_t rshift() {
    *(int32_t*)stack_at(0) /= 2;
    return 0;
}


// ( n -- u)
//Computes absolute value of the top of the stack
int32_t abs(){
    *(int32_t*)stack_at(0) = abs(*(int32_t*)stack_at(0));
    return 0;
}

// ( d -- ud )
int32_t dabs() {
    *(int64_t*)stack_at(0) = abs(*(int64_t*)stack_at(0));
    return 0;
}

// ( n1 -- n2 )
//Negates the top of the stack
int32_t neg(){
    *(int32_t*)stack_at(0) *= -1;
    return 0;
}

// ( d1 -- d2 )
//Negates the double-length top of the stack
int32_t Dneg() {
    *(int64_t*)stack_at(0) *= -1;
    return 0;
}

// ( n1 n2 -- n3 )
//Returns minimum of n1 and n2
int32_t min(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    int32_t b = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(a > b)
        stack_push(b);
    else
        stack_push(a);
    return 0;
}

// ( d1 d2 -- d3 )
// Returns the minimum of 2 double-length numbers
int32_t Dmin(){
    int64_t a = *(int64_t*)stack_at(0);
    stack_pop(2);
    int64_t b = *(int64_t*)stack_at(0);
    stack_pop(2);
    if(a > b)
        stack_push(b);
    else
        stack_push(a);
    return 0;
}

// ( n1 n2 -- n3 )
//Returns maximum of 2 numbers
int32_t max(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    int32_t b = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(a > b)
        stack_push(a);
    else
        stack_push(b);
    return 0;
}

// ( d1 d2 -- d3 )
// Returns the maximum of 2 double-length numbers
int32_t Dmax(){
    int64_t a = *(int64_t*)stack_at(0);
    stack_pop(2);
    int64_t b = *(int64_t*)stack_at(0);
    stack_pop(2);
    if(a > b)
        stack_push(a);
    else
        stack_push(b);
    return 0;
}
