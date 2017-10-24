//
// Created by Max on 2/2/2017.
//

#ifndef MX_FORTH_STACK_H
#define MX_FORTH_STACK_H

#include <cstdlib>

#define uint unsigned


class Stack {
public:
    Stack(size_t bytes) : top(-1), stack(new uint[bytes]) {}
    ~Stack() { delete [] stack; }
    uint* at(int index) const { return stack + (top - index); }
    void pop(int mag);
    void push(int val);
    void clear() { top = -1; }
    int size() { return top + 1; }
private:
    int top; //Site of the top of the stack, counting by intervals of 4 bytes
    uint* stack;
};

int abort_();


#endif //MX_FORTH_STACK_H
