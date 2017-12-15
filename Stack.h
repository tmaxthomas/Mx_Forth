//
// Created by Max on 2/2/2017.
//

#ifndef MX_FORTH_STACK_H
#define MX_FORTH_STACK_H

#include <stdlib.h>
#include <stdint.h>

#define uint unsigned

class Stack {
public:
    Stack(size_t bytes) : top(-1), stack((uint*) malloc(bytes)) {}
    ~Stack() { free(stack); }
    uint* at(int index) const;
    void pop(int mag);
    void push(int val);
    void push(int64_t val);
    void clear() { top = -1; }
    int size() { return top + 1; }
    int top; //Site of the top of the stack, counting by intervals of 4 bytes
    uint* stack;
};

int abort_();


#endif //MX_FORTH_STACK_H
