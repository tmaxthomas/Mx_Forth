#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../stack.h"
#include "../sys.h"

// ( addr u1 -- addr u2 )
int32_t trailing() {
    uint32_t u = *stack_at(0);
    char* str = (char*)*stack_at(1);
    stack_pop(1);
    //Creatively (ab)using for loops for fun and profit
    for(; str[u-1] != ' '; u--);
    str[u] = '\0';
    stack_push((int32_t) u);
    return 0;
}

// ( ud -- ud )
int32_t bracket_pound() {
    sys.swp = (char*) sys.stack + 256;
    sys.swp_len = 0;
    return 0;
}

// ( ud -- addr u )
int32_t pound_bracket() {
    stack_pop(2);
    stack_push((int32_t) sys.swp);
    stack_push((int32_t) sys.swp_len);
    return 0;
}

// ( ud1 -- ud2 )
int32_t pound() {
    uint64_t ud = *(uint64_t*)stack_at(1);
    memmove(sys.swp + 1, sys.swp, sys.swp_len);
    //ASCII table magic
    char digit = (ud % 10) + '0';
    sys.swp[0] = digit;
    *(uint64_t*)stack_at(1) /= 10;
    sys.swp_len++;
    return 0;
}

// ( ud1 -- 0 )
int32_t pounds() {
    uint64_t ud = *(uint64_t*)stack_at(1);
    char buf[32];
    uint32_t len = (uint32_t) sprintf(buf, "%llu", ud);
    memmove(sys.swp + len, sys.swp, sys.swp_len);
    memmove(sys.swp, buf, len);
    *(uint64_t*)stack_at(1) = 0;
    sys.swp_len += len;
    return 0;
}

// ( ud1 c -- ud1 )
int32_t hold() {
    memmove(sys.swp + 1, sys.swp, sys.swp_len);
    sys.swp[0] = *(char*)stack_at(0);
    stack_pop(1);
    sys.swp_len++;
    return 0;
}

// ( ud1 -- ud1 )
int32_t sign() {
    int32_t n = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(n < 0) {
        memmove(sys.swp + 1, sys.swp, sys.swp_len);
        sys.swp[0] = '-';
        sys.swp_len++;
    }
    return 0;
}
