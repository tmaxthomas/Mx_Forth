//
// Created by Max on 2/2/2017.
//

#include "Stack.h"

void Stack::pop() {
    int* st = (int*)stack;
    st[top] = 0;
    top--;
}

void Stack::push(int val) {
    int* st = (int*)stack;
    st[top + 1] = val;
    top++;
}
