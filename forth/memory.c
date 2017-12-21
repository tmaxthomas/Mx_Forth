#include <stdio.h>

#include "memory.h"
#include "output.h"
#include "intmath.h"
#include "../stack.h"
#include "../sys.h"

// ( n addr -- )
// Stores n at the memory location pointed to by addr
void store() {
    int* ptr = (int32_t*)*stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    stack_pop(2);
    *ptr = n;
}

// ( d addr -- )
// Stores d at the memory location pointed to by addr
void store2() {
    int64_t* ptr = (int64_t*)*stack_at(0);
    int64_t n = *(int64_t*)stack_at(2);
    stack_pop(3);
    *ptr = n;
}

// ( c addr -- )
// Stores c at the memory location pointed to by addr
void c_store() {
    char *ptr = (char *) *stack_at(0);
    char n = *(char *) stack_at(1);
    stack_pop(2);
    *ptr = n;
}

// ( n addr -- )
// Adds n to the number stored at addr
void plus_store() {
    int* ptr = (int32_t*)*stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    stack_pop(2);
    *ptr += n;
}

// ( addr -- n )
// Pushes the contents of addr to the top of the stack
void fetch() {
    int* ptr = (int32_t*)*stack_at(0);
    stack_pop(1);
    stack_push(*ptr);
}

// ( addr -- d )
// Pushes the double-length contents of addr to the top of the stack
void fetch2() {
    int64_t* ptr = (int64_t*)*stack_at(0);
    stack_pop(1);
    stack_push(*ptr);
}

// ( addr -- c )
// Pushes the byte pointed to by addr to the top of the stack
void c_fetch() {
    char* ptr = (char*)*stack_at(0);
    stack_pop(1);
    stack_push((int32_t)*ptr);
}

// ( addr  -- n )
// Equivalent to @ .
void query() {
    fetch();
    print();
}

// ( -- 4 )
// Pushes 4 (the size of a cell) onto the stack
void cell() {
    stack_push(4);
}

// ( n1 -- n2 )
// Multiplies n1 by 4
void cells() {
    *stack_at(0) *= 4;
}

// ( addr1 -- addr2 )
// Increments addr1 by the size of a cell
void cell_plus() {
    cell();
    add();
}

// ( n c addr -- )
// Fills n1 bytes at addr with byte c
void fill() {
    char b = *(char*)stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    char* addr = (char*)*stack_at(2);
    stack_pop(3);
    for(int32_t i = 0; i < n; i++)
        addr[i] = b;
}

// ( n addr -- )
// Fills n bytes at addr with 0
void erase() {
    stack_push(0);
    fill();
}

// ( u addr -- )
// Prints the contents of u bytes at addr
void dump() {
    uint32_t c = *stack_at(0);
    int* addr = (int32_t*)stack_at(1);
    stack_pop(2);
    for(uint32_t i = 0; i < c; i++)
        if(i % 4 == 0) printf("%d ", addr[i / 4]);
}

// ( -- addr )
//Pushes the address of the top of the stack onto the stack_q
void sp_at() {
    stack_push((int32_t) stack_at(0));
}

// ( -- addr )
//Pushes the address of the start of the trminal input buffer onto the stack
void tib() {
    stack_push((int32_t) sys.pad);
}

// ( -- u )
//Pushes the a pointer to the length of the terminal input buffer onto the stack
void pound_tib() {
    stack_push((int32_t) &sys.pad_len);
}
