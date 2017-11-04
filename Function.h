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
    Number(std::string str);
    Number(int num, uint size) : n(num), arr_size(size) {}
    //Number(Number* old);
    int run() override;
    int n;
    uint arr_size;
};

//Subclass used to deal with printing strings from within user-defined functions
class StrPrint : public Function {
public:
    StrPrint(char* str_) : Function(), str((char*) malloc(strlen(str_))) {strcpy(str, str_);}
    //StrPrint(StrPrint* old);
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
