#include "../stack.h"

// ( n -- )     rstack: ( -- n )
//Pushes top of stack onto return stack
int32_t retPush(){
    int32_t a = *stack_at(0);
    stack_pop(1);
    rstack_push(a);
    return 0;
}

// ( -- n )     rstack: ( n -- )
//Pushes top of return stack onto stack
int32_t retPop(){
    int32_t a = *rstack_at(0);
    rstack_pop(1);
    stack_push(a);
    return 0;
}

// ( -- n)      rstack: ( -- )
//Copies top of return stack onto stack
int32_t retCopy(){
    int32_t a = *rstack_at(0);
    stack_push(a);
    return 0;
}

// ( -- n )     rstack: ( -- )
//Copies 3rd value on return stack onto stack
int32_t retCopy3(){
    int32_t a = *rstack_at(2);
    stack_push(a);
    return 0;
}
