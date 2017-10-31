//
// Created by Max on 2/2/2017.
//

#include "Stack.h"

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
    else
        abort_();
}
