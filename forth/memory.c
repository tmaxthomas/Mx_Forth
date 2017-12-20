#include <stdio.h>

#include "memory.h"
#include "output.h"
#include "intmath.h"
#include "../stack.h"
#include "../sys.h"

// ( n addr -- )
// Stores n at the memory location pointed to by addr
int32_t store() {
    int* ptr = (int32_t*)*stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    stack_pop(2);
    *ptr = n;
    return 0;
}

// ( d addr -- )
// Stores d at the memory location pointed to by addr
int32_t store2() {
    int64_t* ptr = (int64_t*)*stack_at(0);
    int64_t n = *(int64_t*)stack_at(2);
    stack_pop(3);
    *ptr = n;
    return 0;
}

// ( c addr -- )
// Stores c at the memory location pointed to by addr
int32_t c_store() {
    char *ptr = (char *) *stack_at(0);
    char n = *(char *) stack_at(1);
    stack_pop(2);
    *ptr = n;
    return 0;
}

// ( n addr -- )
// Adds n to the number stored at addr
int32_t plus_store() {
    int* ptr = (int32_t*)*stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    stack_pop(2);
    *ptr += n;
    return 0;
}

// ( addr -- n )
// Pushes the contents of addr to the top of the stack
int32_t fetch() {
    int* ptr = (int32_t*)*stack_at(0);
    stack_pop(1);
    stack_push(*ptr);
    return 0;
}

// ( addr -- d )
// Pushes the double-length contents of addr to the top of the stack
int32_t fetch2() {
    int64_t* ptr = (int64_t*)*stack_at(0);
    stack_pop(1);
    stack_push(*ptr);
    return 0;
}

// ( addr -- c )
// Pushes the byte pointed to by addr to the top of the stack
int32_t c_fetch() {
    char* ptr = (char*)*stack_at(0);
    stack_pop(1);
    stack_push((int32_t)*ptr);
    return 0;
}

// ( addr  -- n )
// Equivalent to @ .
int32_t query() {
    fetch();
    print();
    return 0;
}

// ( -- 4 )
// Pushes 4 (the size of a cell) onto the stack
int32_t cell() {
    stack_push(4);
    return 0;
}

// ( n1 -- n2 )
// Multiplies n1 by 4
int32_t cells() {
    *stack_at(0) *= 4;
    return 0;
}

// ( addr1 -- addr2 )
// Increments addr1 by the size of a cell
int32_t cell_plus() {
    cell();
    add();
    return 0;
}

// ( n c addr -- )
// Fills n1 bytes at addr with byte c
int32_t fill() {
    char b = *(char*)stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    char* addr = (char*)*stack_at(2);
    stack_pop(3);
    for(int32_t i = 0; i < n; i++)
        addr[i] = b;
    return 0;
}

// ( n addr -- )
// Fills n bytes at addr with 0
int32_t erase() {
    stack_push(0);
    return fill();
}

// ( u addr -- )
// Prints the contents of u bytes at addr
int32_t dump() {
    uint32_t c = *stack_at(0);
    int* addr = (int32_t*)stack_at(1);
    stack_pop(2);
    for(uint32_t i = 0; i < c; i++)
        if(i % 4 == 0) printf("%d ", addr[i / 4]);
    return 0;
}

// ( -- addr )
//Pushes the address of the top of the stack onto the stack_q
int32_t sp_at() {
    stack_push((int32_t) stack_at(0));
    return 0;
}

// ( -- addr )
//Pushes the address of the start of the trminal input buffer onto the stack
int32_t tib() {
    stack_push((int32_t) sys.buf);
    return 0;
}

// ( -- u )
//Pushes the a pointer to the length of the terminal input buffer onto the stack
int32_t pound_tib() {
    stack_push((int32_t) &sys.buf_len);
    return 0;
}
