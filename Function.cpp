//
// Created by Max on 2/7/2017.
//

#include <iostream>
#include <unordered_map>
#include "Function.h"

extern std::unordered_map<Function*, Function*> copy_map;
extern struct System sys;

//Call the function pointer, then use its return value to determine what to run next
//Yes, it seems pointless, but it's needed due to the way inherited classes use the method.
int32_t Function::run() {
    return fxn();
}

int32_t Var::run() {
    stack_push(n);
    return 0;
}

int32_t DoubleConst::run() {
    stack_push(n64);
    return 0;
}

//Prints the stored string. Simple enough.
int32_t StrPrint::run() {
    printf("%s", str);
    return 0;
}

//Prints the stored string and aborts
int32_t Abort::run() {
    printf("%s", str);
    abort_();
    return 0;
}

int32_t UsrFunc::run() {
    Function* func = head;
    while(func->next) {
        int32_t idx = func->run();
        func = func->next[idx];
    }
    return func->run();
}

//Number determination helper function
bool is_num(std::string& str) {
    for(uint32_t i = 0; i < str.size(); i++)
        if((str[i] < '0' || str[i] > '9') && (i != str.size() - 1 || str[i] != '.') && (i != 0 || str[i] != '-'))
            return false;
    return true;
}
