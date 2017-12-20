#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string>

#include "../stack.h"
#include "../sys.h"
#include "output.h"

//( -- )
//Prints a newline character to the terminal
int32_t cr() {
    printf("\n");
    return 0;
}

//( n -- )
//Prints n spaces
int32_t spaces() {
    std::string str((unsigned long)*(int32_t*)stack_at(0), ' ');
    printf("%s", str.c_str());
    stack_pop(1);
    return 0;
}

//( -- )
//Prints a space
int32_t space() {
    printf(" ");
    return 0;
}

//( c -- )
//Prints character c
int32_t emit() {
    char ch = (char)*stack_at(0);
    printf("%c", ch);
    stack_pop(1);
    return 0;
}

// ( n -- )
//Prints and pops the top of the stack, followed by a space
int32_t print() {
    printf("%d ", *(int32_t*)stack_at(0));
    stack_pop(1);
    return 0;
}

// ( u -- )
//Unsigned int32_t print
int32_t uprint() {
    printf("%u ", *stack_at(0));
    stack_pop(1);
    return 0;
}

// ( d -- )
//Double length integer print
int32_t dprint() {
    printf("%lld ", *(int64_t*)stack_at(0));
    stack_pop(2);
    return 0;
}

// ( u1 u2 -- )
//Unsigned right-justified print
int32_t urjprint() {
    uint32_t size = *stack_at(0);
    uint32_t data =  *stack_at(1);
    uint32_t num_spaces = size - (uint32_t) floor(log10((float)data));
    stack_pop(2);
    for(uint32_t i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%u", data);
    return 0;
}

// ( addr u -- )
int32_t type() {
    uint32_t u = *stack_at(0);
    char* str = (char*)*stack_at(1);
    stack_pop(2);
    fwrite(str, 1, u, stdout);
    return 0;
}

// ( d u -- )
// Double-length right-justified print
int32_t drjprint() {
    uint32_t size = *stack_at(0);
    int64_t data = *(int64_t*)stack_at(1);
    int64_t num_spaces = size - (int64_t) floor(log10((float)data));
    stack_pop(3);
    for(int64_t i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%lld ", data);
    return 0;
}

// ( -- )
//Prints the contents of the stack
int32_t printS() {
    for(uint32_t* a = sys.stack; a != sys.stack_0; a++) {
        printf("%d ", *(int32_t*) a);
    }
    return 0;
}
