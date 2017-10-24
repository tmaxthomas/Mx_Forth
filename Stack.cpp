//
// Created by Max on 2/2/2017.
//

#include "Stack.h"

void Stack::push(int val) {
    int* st = (int*)stack;
    top++;
    st[top] = val;
}

void Stack::pop(int mag) {
    if(top > -1)
        top -= mag;
    else
        abort_();
}
