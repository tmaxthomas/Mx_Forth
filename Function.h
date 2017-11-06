//
// Created by Max on 2/7/2017.
//

#ifndef MX_FORTH_FUNCTION_H
#define MX_FORTH_FUNCTION_H

#include <stdint.h>
#include <cstring>
#include "Stack.h"

#define byte uint8_t

//Class used in storing user defined functions. User-defined functions are stored as graphs, with elements of class Function (and class Number) at the
//nodes.
class Function {
public:
    Function() : fxn(NULL), next(NULL) {}
    Function(int(*fxn_)()) : fxn(fxn_), next(NULL) {}
    Function(Function* old) : fxn(old->fxn), next(NULL) {}
    virtual ~Function() { delete [] next; }
    int(*fxn)();
    Function** next;
    virtual int run();
};

//Subclass used to deal with pushing numbers onto the stack from within user-defined functions
class Number : public Function {
public:
    Number() : n(0) {}
    Number(std::string str);
    int run() override;
    int n;
};

//Subclass used to deal with all variables, arrays, and single-length constants
class Var : public Number {
public:
    Var(int n_, size_t s_) : Number::n(n_), s(s_) {}
    size_t s;
};

//Subclass used to deal with double-length constants
class DoubleConst : public Number {
public:
    DoubleConst(int64_t n_) : n64(n_) {}
    int64_t n64;
    int run() override;
};

//Subclass used to deal with printing strings from within user-defined functions
class StrPrint : public Function {
public:
    StrPrint(char* str_) : Function(), str((char*) malloc(strlen(str_))) {strcpy(str, str_);}
    int run() override;
    char* str;
};

class Abort : public Function {
public:
    Abort(char* str_) : Function(), str((char*) malloc(strlen(str_))) { strcpy(str, str_); }
    int run() override;
    char* str;
};

class UsrFunc : public Function {
public:
    UsrFunc(Function* head_) : Function(), head(head_) {}
    Function* head;
    int run() override;
};

bool is_num(std::string& str);

#endif //MX_FORTH_FUNCTION_H
