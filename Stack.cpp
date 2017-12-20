//
// Created by Max on 2/2/2017.
//

#include "stack.h"
#include "sys.h"

extern bool ABORT, BYE, QUIT, S_UND;

/*
uint* Stack::at(int index) const {
    if(top > -1)
        return stack + (top - index);
    else {
        S_UND = true;
        abort_();
    }
    return NULL;
}

void Stack::push(int val) {
    top++;
    *(int*)(stack + top) = val;
}

void Stack::push(int64_t val) {
    top++;
    *(int64_t*)(stack + top) = val;
    top++;
}

void Stack::pop(int mag) {
    if(top > -1)
        top -= mag;
    else {
        S_UND = true;
        abort_();
    }
}
*/

uint32_t* stack_at(uint32_t index) {
    return sys.stack + index;
}

void stack_push(int32_t val) {
    sys.stack--;
    *(int32_t*)sys.stack = val;
}

void stack_push(int64_t val) {
    sys.stack -= 2;
    *(int64_t*)sys.stack = val;
}

void stack_pop(int32_t mag) {
    sys.stack += mag;
}

void stack_clear() {
    sys.stack = sys.stack_0;
}

uint32_t* rstack_at(uint32_t index) {
    return sys.rstack + index;
}

void rstack_push(int32_t val){
    sys.rstack--;
    *(int32_t*)sys.rstack = val;
}

void rstack_pop(int mag){
    sys.rstack += mag;
}

void rstack_clear() {
    sys.rstack = sys.rstack_0;
}
