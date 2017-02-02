//
// Created by Max on 2/2/2017.
//

#ifndef MX_FORTH_STACK_H
#define MX_FORTH_STACK_H

#include <cstdlib>


class Stack {
public:
    Stack(int bytes) : stack(malloc(bytes)) {}
    ~Stack() { free(stack); }
    void* at(int index) { return &stack[top - index]; }
    void pop();
    void push(int val);
    int size() { return top + 1; }
private:
    int top; //Site of the top of the stack, counting by intervals of 4 bytes
    void* stack;
};


#endif //MX_FORTH_STACK_H
