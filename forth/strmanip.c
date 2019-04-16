#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "../stack.h"
#include "../sys.h"
#include "control.h"
#include "strmanip.h"

// ( ud -- ud )
void bracket_pound() {
    sys.pad = (char *) sys.stack + 256;
    sys.pad_len = 0;
}

// ( ud -- addr u )
void pound_bracket() {
    stack_pop(2);
    stack_push(forth_addr((uint32_t *) sys.pad));
    stack_push(sys.pad_len);
}

// ( ud1 -- ud2 )
void pound() {
    uint64_t ud = *(uint64_t *) stack_at(1);
    memmove(sys.pad + 1, sys.pad, sys.pad_len);
    // ASCII table magic
    char digit = (ud % sys.base) + '0';
    sys.pad[0] = digit;
    *(uint64_t *) stack_at(1) /= sys.base;
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
    sys.pad[0] = *(char *) stack_at(0);
    stack_pop(1);
    sys.pad_len++;
}

// ( ud1 -- ud1 )
void sign() {
    int32_t n = *(int32_t *) stack_at(0);
    stack_pop(1);
    if(n < 0) {
        memmove(sys.pad + 1, sys.pad, sys.pad_len);
        sys.pad[0] = '-';
        sys.pad_len++;
    }
}

// ( ud1 c-addr u1 -- ud2 c-addr u2 )
void to_number() {
    uint32_t n = *stack_at(0);
    char *str = (char *) sys_addr(*stack_at(1));
    uint64_t b = *(uint64_t *) stack_at(2);
    stack_pop(4);

    int i, num;
    for(i = 0; i < n && ((num = to_num(*str)) != -1); i++, str++) {
        b *= sys.base;
        b += n;
    }

    stack_push_d(n);
    stack_push((int32_t) str);
    stack_push(n - i);
}

// ( -- addr )
// TODO: This will not work until I rejigger the system memory arrangement
void to_in() {
    stack_push(forth_addr(&sys.idx_loc));
}

int is_quote(int ch) {
    return ch == '\"';
}

// ( -- c )
void bl() {
    stack_push((int32_t) ' ');
}

// ( -- c )
void char_() {
    char *buf = get_substring(isspace);
    stack_push(buf[0]);
    free(buf);
}

// ( c-addr -- c-addr c )
void count() {
    char *len = (char *) sys_addr(*stack_at(0));
    (*stack_at(0))++;
    stack_push((int32_t) *len);
}

// ( -- )
void dot_quote() {
    char *buf = get_substring(is_quote);
    *sys.cp = (uint32_t) dot_quote_runtime;
    sys.cp++;
    char *ccp = (char *) sys.cp;
    *ccp = strlen(buf);
    memcpy(ccp + 1, buf, *ccp);
    int count = *ccp + 1;
    sys.cp += count / 4;
    if (count % 4 != 0) {
        sys.cp++;
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
    if((*c + 1) % 4 == 0) {
        sys.inst--;
    }
    free(buf);
}

// ( -- )
void s_quote() {
    char *buf = get_substring(is_quote);
    *sys.cp = (uint32_t) s_quote_runtime;
    sys.cp++;
    char *ccp = (char *) sys.cp;
    *ccp = strlen(buf);
    memcpy(ccp + 1, buf, *ccp);
    int count = *ccp + 1;
    sys.cp += count / 4;
    if (count % 4 != 0) {
        sys.cp++;
    }
    free(buf);
}

void s_quote_runtime() {
    sys.inst++;
    char *c = (char *) sys.inst;
    stack_push(forth_addr((uint32_t *) (c + 1)));
    stack_push((int32_t) *c);
    sys.inst += ((*c + 1) / 4);
    if((*c + 1) % 4 == 0) {
        sys.inst--;
    }
}

// ( -- addr n )
void source() {
    stack_push(forth_addr((uint32_t *) sys.idx));
    stack_push((int32_t) sys.idx_len);
}

int word_delim = '\0';

int is_word_delim(int c) {
    return c == word_delim;
}


void word() {
    word_delim = *sys.idx;
    char *buf = get_substring(is_word_delim);
    sys.pad = (char *) sys.stack + 256;
    sys.pad[0] = strlen(buf);
    memcpy(sys.pad + 1, buf, sys.pad[0]);
    sys.pad[sys.pad[0] + 2] = ' ';
    stack_push(forth_addr((uint32_t *) sys.pad));
}
