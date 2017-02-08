//
// Created by Max on 2/7/2017.
//

#include <iostream>
#include "Function.h"

void Function::operator() () {
    int idx = fxn();
    if(next) (*next[idx])();
}

void Number::operator()() {
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
}
