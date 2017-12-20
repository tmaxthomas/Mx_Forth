//
// Created by Max on 2/2/2017.
//

#ifndef MX_FORTH_STACK_H
#define MX_FORTH_STACK_H

#include <stdlib.h>
#include <stdint.h>

/*
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
*/

int abort_();

uint32_t* stack_at(uint32_t index);
void stack_push(int32_t val);
void stack_push(int64_t val);
void stack_pop(int32_t mag);
void stack_clear();

uint32_t* rstack_at(uint32_t index);
void rstack_push(int32_t val);
void rstack_pop(int32_t mag);
void rstack_clear();

#endif //MX_FORTH_STACK_H
