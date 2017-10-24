//
// Created by Max on 2/2/2017.
//

#include "Stack.h"

void Stack::push(int val) {
    int* st = (int*)stack;
    top++;
    st[top] = val;
}
