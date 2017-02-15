//
// Created by Max on 2/7/2017.
//

#ifndef MX_FORTH_FUNCTION_H
#define MX_FORTH_FUNCTION_H


#include <string>

#include "Stack.h"

class Function {
public:
    Function() : stack(NULL) {}
    Function(int(*fxn_)()) : fxn(fxn_) {}
    Function(Function& old) : fxn(old.fxn) {}
    int(*fxn)();
    Function** next;
    void operator()();
};

class Number : Function {
public:
    Number(std::string str_, Stack* stack_) : str(str_), stack(stack_) {}
    void operator()();
    std::string str;
};


#endif //MX_FORTH_FUNCTION_H
