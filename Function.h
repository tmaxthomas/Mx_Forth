//
// Created by Max on 2/7/2017.
//

#ifndef MX_FORTH_FUNCTION_H
#define MX_FORTH_FUNCTION_H


#include <string>
#include "Stack.h"

class Function {
public:
    Function() : next(NULL) {}
    Function(int(*fxn_)()) : fxn(fxn_), next(NULL) {}
    Function(Function* old);
    ~Function() { delete [] next; }
    int(*fxn)();
    Function** next;
    virtual void run();
};

class Number : public Function {
public:
    Number(std::string str_) : str(str_) {}
    Number(Number* old);
    ~Number() { delete [] next; }
    void run() override;
    std::string str;
};


#endif //MX_FORTH_FUNCTION_H