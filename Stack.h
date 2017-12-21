//
// Created by Max on 2/2/2017.
//

#ifndef MX_FORTH_STACK_H
#define MX_FORTH_STACK_H

#include <stdlib.h>
#include <stdint.h>

uint32_t* stack_at(uint32_t index);
void stack_push(int32_t val);
void stack_push_d(int64_t val);
void stack_pop(int32_t mag);
void stack_clear();

uint32_t* rstack_at(uint32_t index);
void rstack_push(int32_t val);
void rstack_push_d(int64_t val);
void rstack_pop(int32_t mag);
void rstack_clear();

#endif //MX_FORTH_STACK_H
