#include <stdio.h>

#include "stack.h"
#include "sys.h"
#include "forth/control.h"

uint32_t* stack_at(uint32_t index) {
    if(sys.stack == sys.stack_0) {
        if(!sys.ABORT) fprintf(stderr, "ERROR: Stack underflow detected, aborting\n");
        abort_();
    }
    return sys.stack + index;
}

void stack_push(int32_t val) {
    if(!sys.ABORT) {
        sys.stack--;
        *(int32_t*)sys.stack = val; 
    }
}

void stack_push_d(int64_t val) {
    if(!sys.ABORT) {
        sys.stack -= 2;
        *(int64_t*)sys.stack = val;
    }
}

void stack_pop(int32_t mag) {
    sys.stack += mag;
    if(sys.stack > sys.stack_0) {
        if(!sys.ABORT) fprintf(stderr, "ERROR: Stack underflow detected, aborting\n");
        abort_();
    }
}

void stack_clear() {
    sys.stack = sys.stack_0;
}

uint32_t* rstack_at(uint32_t index) {
    if(sys.rstack == sys.rstack_0) {
        if(!sys.ABORT) fprintf(stderr, "ERROR: R-Stack underflow detected, aborting\n");
        abort_();
    }
    return sys.rstack + index;
}

void rstack_push(int32_t val) {
    if(!sys.ABORT) {
        sys.rstack--;
        *(int32_t*)sys.rstack = val;
    }
}

void rstack_push_d(int64_t val) {
    if(!sys.ABORT) {
        sys.rstack -= 2;
        *(int64_t*)sys.rstack = val;
    }
}

void rstack_pop(int mag) {
    sys.rstack += mag;
    if(sys.stack > sys.stack_0) {
        if(!sys.ABORT) fprintf(stderr, "ERROR: R-Stack underflow detected, aborting\n");
        abort_();
    }
}

void rstack_clear() {
    sys.rstack = sys.rstack_0;
}
