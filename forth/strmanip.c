#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../stack.h"
#include "../sys.h"
#include "control.h"
#include "strmanip.h"

// ( addr u1 -- addr u2 )
void trailing() {
    uint32_t u = *stack_at(0);
    char* str = (char*)*stack_at(1);
    stack_pop(1);
    //Creatively (ab)using for loops for fun and profit
    for(; str[u-1] != ' '; u--);
    str[u] = '\0';
    stack_push((int32_t) u);
}

// ( ud -- ud )
void bracket_pound() {
    sys.pad = (char *) sys.stack + 256;
    sys.pad_len = 0;
}

// ( ud -- addr u )
void pound_bracket() {
    stack_pop(2);
    stack_push((int32_t) sys.pad);
    stack_push((int32_t) sys.pad_len);
}

// ( ud1 -- ud2 )
void pound() {
    uint64_t ud = *(uint64_t *) stack_at(1);
    memmove(sys.pad + 1, sys.pad, sys.pad_len);
    //ASCII table magic
    char digit = (ud % sys.base) + '0';
    sys.pad[0] = digit;
    *(uint64_t*)stack_at(1) /= sys.base;
    sys.pad_len++;
}

// ( ud1 -- 0 )
void pounds() {
    uint64_t *ud = (uint64_t *) stack_at(1);
    while(*ud > 0) {
        pound();
    } 
}

// ( ud1 c -- ud1 )
void hold() {
    memmove(sys.pad + 1, sys.pad, sys.pad_len);
    sys.pad[0] = *(char*)stack_at(0);
    stack_pop(1);
    sys.pad_len++;
}

// ( ud1 -- ud1 )
void sign() {
    int32_t n = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(n < 0) {
        memmove(sys.pad + 1, sys.pad, sys.pad_len);
        sys.pad[0] = '-';
        sys.pad_len++;
    }
}

// ( ud1 c-addr u1 -- ud2 c-addr u2 )
void to_number() {
    
}

void parse() {
    
}

int is_quote(int ch) {
    return ch == '\"';
}

void dot_quote() {
    char *buf = get_substring(is_quote);
    if(sys.COMPILE) {
        *sys.cp = (uint32_t) dot_quote_runtime;
        sys.cp++;
        char *ccp = (char *) sys.cp;
        *ccp = strlen(buf);
        memcpy(ccp + 1, buf, *ccp);
        int count = *ccp + 1;
        sys.cp += count / 4;
        if (count % 4 != 0) sys.cp++;
    } else {
        printf("%s", buf);
    }
    free(buf);
}

void dot_quote_runtime() {
    sys.inst++;
    char *c = (char *) sys.inst;
    char *buf = malloc(*c + 1);
    memcpy(buf, c + 1, *c);
    buf[(int) *c] = '\0';
    printf("%s", buf);
    sys.inst += ((*c + 1) / 4);
    if((*c + 1) % 4 == 0)
        sys.inst--;
    free(buf);
}
