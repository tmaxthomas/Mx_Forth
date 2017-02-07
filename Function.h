//
// Created by Max on 2/7/2017.
//

#ifndef MX_FORTH_FUNCTION_H
#define MX_FORTH_FUNCTION_H


class Function {
    void(*fxn)();
    Function** next;
};


#endif //MX_FORTH_FUNCTION_H
