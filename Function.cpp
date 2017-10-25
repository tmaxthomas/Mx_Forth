//
// Created by Max on 2/7/2017.
//

#include <iostream>
#include <unordered_map>
#include "Function.h"

extern std::unordered_map<Function*, Function*> copy_map;

extern Stack *stack;

//Copy constructor
Function::Function(Function* old) : fxn(old->fxn), branches(old->branches) {
    copy_map.insert(std::make_pair(old, this));
    if(old->next) {
        next = new Function*[branches];
        for (int a = 0; a < branches; a++) {
            Number *old_num = dynamic_cast<Number*>(old->next[a]); //Abusing dynamic_cast for fun and profit
            StrPrint *old_strpr = dynamic_cast<StrPrint*>(old->next[a]);
            if (!old_num && !old_strpr) {
                std::unordered_map<Function*, Function*>::iterator itr = copy_map.find(old->next[a]);
                if (itr == copy_map.end()) {
                    next[a] = new Function(old->next[a]);
                } else {
                    next[a] = itr->second;
                }
            } else if (old_num) {
                next[a] = new Number(old_num);
            } else {
                next[a] = new StrPrint(old_strpr);
            }
        }
    } else
        next = NULL;
}

//Copy constructor
Number::Number(Number* old) : Function(), str(old->str) {
    if(old->next) {
        next = new Function*[1];
        Number* old_num = dynamic_cast<Number*>(old->next[0]);
        StrPrint* old_strpr = dynamic_cast<StrPrint*>(old->next[0]);
        //If the next node is a number
        if(old_num)
            next[0] = new Number(old_num);
        else if(old_strpr) //Or if the next node is a string print
            next[0] = new StrPrint(old_strpr);
        else
            next[0] = new Function(old->next[0]);
    }
}

//Copy constructor (Ain't this familiar...)
StrPrint::StrPrint(StrPrint* old) : Function(), str(old->str) {
    if(old->next) {
        next = new Function*[1];
        Number* old_num = dynamic_cast<Number*>(old->next[0]);
        StrPrint* old_strpr = dynamic_cast<StrPrint*>(old->next[0]);
        //If the next node is a number
        if(old_num)
            next[0] = new Number(old_num);
        else if(old_strpr) //Or if the next node is a string print
            next[0] = new StrPrint(old_strpr);
        else
            next[0] = new Function(old->next[0]);
    }
}


//Call the function pointer, then use its return value to determine what to run next
//Yes, it seems pointless, but it's needed due to the way inherited classes use the method.
int Function::run() {
    return fxn();
}
//Basically just the number function from main.cpp
int Number::run() {
    if(!is_num(str)) {
        std::cout << str << " ?";
        exit(1);
    }
    int n = atoi(str.c_str());
    stack->push(n);
    return 0;
}

//Prints the stored string. Simple enough.
int StrPrint::run() {
    std::cout << str;
    return 0;
}

//Prints the stored string and aborts
int Abort::run() {
    std::cout << str;
    abort_();
    return 0;
}

//Number determination helper function
bool is_num(std::string& str) {
    for(uint i = 0; i < str.size(); i++)
        if(str[i] < '0' || str[i] > '9')
            return false;
    return true;
}