//
// Created by Max on 2/2/2017.
//

#include "Stack.h"

uint* Stack::at(int index) const {
    return stack + (top - index);
}

void Stack::pop(int mag) {
    top -= mag;
}

void Stack::push(int val) {
    int* st = (int*)stack;
    top++;
    st[top] = val;
}

void Stack::push(long val) {
    top += 2;
    long* lt = (long*)&stack[top - 1];
    *lt = val;
}
