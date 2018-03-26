#include "../stack.h"

// ( n -- )     rstack: ( -- n )
//Pushes top of stack onto return stack
void retPush(){
    int32_t a = *(int32_t *) stack_at(0);
    stack_pop(1);
    rstack_push(a);
}

// ( -- n )     rstack: ( n -- )
//Pushes top of return stack onto stack
void retPop(){
    int32_t a = *(int32_t *) rstack_at(0);
    rstack_pop(1);
    stack_push(a);
}

// ( -- n)      rstack: ( -- )
//Copies top of return stack onto stack
void retCopy(){
    int32_t a = *(int32_t *) rstack_at(0);
    stack_push(a);
}

// ( -- n )     rstack: ( -- )
//Copies 3rd value on return stack onto stack
void retCopy3(){
    int32_t a = *(int32_t *) rstack_at(2);
    stack_push(a);
}
