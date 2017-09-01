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
void Function::run() {
    int idx = fxn();
    if(next) next[idx]->run();
}
//Basically just the number function from main.cpp
void Number::run() {
    if(str.size() == 1) {
        if(str.at(0) >= '0' && str.at(0) <= '9')
            stack->push((int)std::stod(str));
        else {
            stack->push(str.at(0));
        }
    } else {
        size_t a;
        double v;
        v = stod(str, &a);
        if(a < str.size()) {
            exit(1);
        } else {
            stack->push((int)v);
        }
    }
    if(next){
        (*next)->run();
    }
}

//Prints the stored string. Simple enough.
void StrPrint::run() {
    std::cout << str;
    if(next)
        (*next)->run();
}