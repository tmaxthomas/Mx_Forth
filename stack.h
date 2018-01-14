#include <stdint.h>

//Data stack ops

uint32_t* stack_at(uint32_t index);
void stack_push(int32_t val);
void stack_push_d(int64_t val);
void stack_pop(int32_t mag);
void stack_clear();

//Return stack ops

uint32_t* rstack_at(uint32_t index);
void rstack_push(int32_t val);
void rstack_push_d(int64_t val);
void rstack_pop(int mag);
void rstack_clear();
