//
// Created by Max on 2/7/2017.
//

#include <iostream>
#include <unordered_map>
#include "Function.h"

extern std::unordered_map<Function*, Function*> copy_map;

extern Stack *stack, *return_stack;

//Copy constructor
Function::Function(Function* old) : fxn(old->fxn), branches(old->branches) {
    copy_map.insert(std::make_pair(old, this));
    if(old->next) {
        next = new Function*[branches];
        for (int a = 0; a < branches; a++) {
            Number *old_num = dynamic_cast<Number*>(old->next[a]); //Abusing dynamic_cast for fun and profit
            if (!old_num) {
                std::unordered_map<Function*, Function*>::iterator itr = copy_map.find(old->next[a]);
                if (itr == copy_map.end()) {
                    next[a] = new Function(old->next[a]);
                } else {
                    next[a] = itr->second;
                }
            } else {
                next[a] = new Number(old_num);
            }
        }
    } else
        next = NULL;
}

//Copy constructor
Number::Number(Number* old) : Function(), str(old->str) {
    next = new Function*[1];
    next[0] = new Function(old->next[0]);
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
