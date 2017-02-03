//
// Created by Max on 2/2/2017.
//

#include "Stack.h"

void* Stack::at(int index) {
    int* st = (int*)stack;
    void* ptr = (void*)&st[top - index]; //Don't even ask why this is necessary. I don't know. Typecasting fucks up big time if I don't do this.
    return ptr;
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
    int* st = (int*)stack;
    long* lt = (long*)st[top];
    void* ptr = &val;
    top += 2;
    *lt = *(long*)ptr;
}
