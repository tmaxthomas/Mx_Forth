#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "../sys.h"
#include "../stack.h"
#include "control.h"

// HELPER FUNCTIONS

//Compares two counted strings for equality
bool str_eq(uint8_t* c1, uint8_t* c2) {
    for(uint8_t i = 0; i <= *c1; i++)
        if(c1[i] != c2[i])
            return false;
    return true;
}

//Returns a pointer to the start of the code section for func
uint32_t* get_xt(uint32_t* func) {
    uint8_t len = *(((uint8_t*) func) + 1);
    uint32_t xt = ((uint32_t) func) + len + 6;
    return (uint32_t*) xt;
}

//FORTH FUNCTIONS

// ( c-addr -- c-addr 0 | xt 1 | xt -1 )
//Finds words in the glossary
void find() {
    uint8_t* name = (uint8_t*)*stack_at(0);
    stack_pop(1);
    uint32_t* gloss_loc = sys.gloss_head;
    while(*gloss_loc) {
        uint8_t* ccp = (uint8_t*) gloss_loc;
        if(str_eq(ccp + 1, name)) {
            stack_push((int32_t) get_xt(gloss_loc));
            //Push something onto the stack for immediacy (not yet implemented)
            stack_push(1);
            return;
        } else {
            uint8_t len = *(ccp + 1);
            //Locate back pointer and assign dereferenced value to gloss_loc
            uint32_t bp = ((uint32_t) gloss_loc) + len + 2;
            gloss_loc = *((uint32_t**) bp);
        }
    }
    stack_push((int32_t) name);
    stack_push(0);
}

// ( -- addr )
// Pushes the exectuion address of the next word in the input stream onto the stack
void tick() {
    char *tmp_buf, *tmp_idx;
    size_t i;
    GetSubstring(isspace(*tmp_idx));
    find(tmp_buf);
    uint32_t* xt = get_xt(stack_at(1));
    stack_pop(2);
    stack_push((uint32_t) xt);
    free(tmp_buf);
}

// ( addr -- )
// Executes the word pointed to by addr by moving the instruction pointer
void execute() {
    uint32_t* xt_ptr = (uint32_t*)*stack_at(0);
    stack_pop(1);
    rstack_push((int32_t) sys.inst);
    sys.inst = xt_ptr;
}

// ( -- )
//Sets the instruction pointer to the top of the rstack, then pops the rstack
void exit_() {
    sys.inst = (uint32_t*)*rstack_at(0);
    rstack_pop(1);
}

void if_(){ }
void else_(){ }
void then(){ }
void do_(){ }
void do_runtime() { }
void loop(){ }
void begin(){ }
void while_(){ }
void repeat(){ }
void until(){ }
void again(){ }
void jump(){ }
void cond_jump(){ }
