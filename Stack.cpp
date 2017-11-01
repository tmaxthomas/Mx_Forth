//
// Created by Max on 2/2/2017.
//

#include "Stack.h"

extern bool ABORT, BYE, QUIT, S_UND;

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
