//
// Created by Max on 2/7/2017.
//

#ifndef MX_FORTH_FUNCTION_H
#define MX_FORTH_FUNCTION_H

#include <string>
#include <stdint.h>
#include "Stack.h"

#define byte uint8_t

//Class used in storing user defined functions. User-defined functions are stored as graphs, with elements of class Function (and class Number) at the
//nodes.
class Function {
public:
    Function() : fxn(NULL), next(NULL), branches(1) {}
    Function(int(*fxn_)()) : fxn(fxn_), next(NULL), branches(1) {}
    Function(Function* old);
    virtual ~Function() { delete [] next; }
    int(*fxn)();
    Function** next;
    byte branches;
    virtual void run();
};
//Subclass used to deal with pushing numbers onto the stack from within user-defined functions
class Number : public Function {
public:
    Number(std::string str_) : Function(), str(str_){}
    Number(Number* old);
    ~Number() override { delete [] next; }
    void run() override;
    std::string str;
};
//Subclass used to deal with printing strings from within user-defined functions
class StrPrint : public Function {
public:
    StrPrint(std::string str_) : Function(), str(str_) {}
    StrPrint(StrPrint* old);
    ~StrPrint() override { delete [] next; }
    void run() override;
    std::string str;
};


#endif //MX_FORTH_FUNCTION_H
