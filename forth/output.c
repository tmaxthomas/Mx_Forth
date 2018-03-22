#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../stack.h"
#include "../sys.h"
#include "output.h"

//( -- )
//Prints a newline character to the terminal
void cr() {
    printf("\n");
    fflush(stdout);
}

//( n -- )
//Prints n spaces
void spaces() {
    for(uint32_t i = 0; i < *(uint32_t*)stack_at(0); i++)
        printf(" ");
    stack_pop(1);
    fflush(stdout);
}

//( -- )
//Prints a space
void space() {
    printf(" ");
    fflush(stdout);
}

//( c -- )
//Prints character c
void emit() {
    char ch = (char)*stack_at(0);
    printf("%c", ch);
    stack_pop(1);
    fflush(stdout);
}

// ( n -- )
//Prints and pops the top of the stack, followed by a space
void print() {
    printf("%d ", *(int32_t*)stack_at(0));
    stack_pop(1);
    fflush(stdout);
}

// ( u -- )
//Unsigned int32_t print
void uprint() {
    printf("%u ", *stack_at(0));
    stack_pop(1);
    fflush(stdout);
}

// ( d -- )
//Double length integer print
void dprint() {
    printf("%lld ", *(int64_t*)stack_at(0));
    stack_pop(2);
    fflush(stdout);
}

// ( u1 u2 -- )
//Unsigned right-justified print
void urjprint() {
    uint32_t size = *stack_at(0);
    uint32_t data =  *stack_at(1);
    uint32_t num_spaces = size - (uint32_t) floor(log10((float)data));
    stack_pop(2);
    for(uint32_t i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%u", data);
    fflush(stdout);
}

// ( addr u -- )
void type() {
    uint32_t u = *stack_at(0);
    char* str = (char*)*stack_at(1);
    stack_pop(2);
    fwrite(str, 1, u, stdout);
    fflush(stdout);
}

// ( d u -- )
// Double-length right-justified print
void drjprint() {
    uint32_t size = *stack_at(0);
    int64_t data = *(int64_t*)stack_at(1);
    int64_t num_spaces = size - (int64_t) floor(log10((float)data));
    stack_pop(3);
    for(int64_t i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%lld ", data);
    fflush(stdout);
}

// ( -- )
//Prints the contents of the stack
void printS() {
    for(uint32_t* a = sys.stack; a != sys.stack_0; a++)
        printf("%d ", *(int32_t*) a);
    fflush(stdout);
}
