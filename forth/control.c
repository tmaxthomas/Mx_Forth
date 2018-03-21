#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../sys.h"
#include "../stack.h"
#include "control.h"



/* HELPER FUNCTIONS */

int64_t ipow(int base, int exp) {
    int64_t ret = 1;
    for(int i = 0; i < exp; i++)
        ret *= base;
    
    return ret;
}

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

//Finds a c string in the dictionary, if it exists
int32_t cfind(char *str, int *precedence) {
    char* fstr = malloc(strlen(str) + 1);
    fstr[0] = strlen(str);
    memcpy(fstr + 1, str, strlen(str));
    stack_push((int32_t) fstr);
    find();
    free(fstr);
    if(*stack_at(0) == 0) {
        stack_pop(2);
        return 0;
    } else {
        if(precedence) {
            *precedence = *stack_at(0);                
        }
        int32_t ret = *stack_at(1);
        stack_pop(2);
        return ret;
    }
}

/* FORTH FUNCTIONS */

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
            if(*ccp == 0) { // Not immediate
                stack_push(1); 
            } else { // Immediate
                stack_push(-1);
            }
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
    int32_t xt = cfind(tmp_buf, NULL);
    stack_push(xt);
    free(tmp_buf);
}

// ( addr -- )
// Executes the word pointed to by addr by moving the instruction pointer
void execute() {
    uint32_t* xt_ptr = (uint32_t*)*stack_at(0);
    stack_pop(1);
    rstack_push((int32_t) (sys.inst + 1));
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

void num_runtime() {
    sys.inst++;
    stack_push(*sys.inst);
}

void dnum_runtime() {
    sys.inst++;
    stack_push_d(*(int64_t *) sys.inst);
    sys.inst++;
}

void loop(){ }
void begin(){ }
void while_(){ }
void repeat(){ }
void until(){ }
void again(){ }
void jump(){ }
void cond_jump(){ }
void lbracket(){ }
void rbracket(){ }
void colon(){
    
}

void semicolon(){ }

void abort_(){ 
    stack_clear();
    rstack_push((int32_t) sys.q_addr);
}

void abort_quote() {
    char *tmp_buf, *tmp_idx;
    size_t i;
    GetSubstring(*tmp_idx == '\"');
    fprintf(stdout, "%s\n", tmp_buf);
    free(tmp_buf);
    abort_();
}

//Word that runs the FORTH system, including the interpreter/compiler
//Well, for now just the interpreter. But you get the point.
void quit() {
    char *tmp_buf, *tmp_idx;
    size_t i, num_bytes;

    rstack_clear();
    rstack_push(0);
    rstack_push((int32_t) sys.q_addr);

    num_bytes = read(0, sys.tib, sys.tib_len);
    sys.tib[num_bytes] = '\0';
    sys.idx = sys.tib;
    sys.idx_loc = 0;

    GetSubstring(isspace(*tmp_idx));
    int precedence;
    int32_t wd = cfind(tmp_buf, &precedence);
    if(wd) {
        free(tmp_buf);
        stack_push(wd);
        execute();
    } else { //Woo, number conversion (yes, I know it's a mess)
        int i = 0, neg = 0, di = 0;
        if(tmp_buf[strlen(tmp_buf)] == '.') {
            int64_t num = 0;
            tmp_buf[strlen(tmp_buf)] = '\0';
            if(isdigit(tmp_buf[i])) {
                num += (tmp_buf[i] - '0') * ipow(sys.base, di++); 
            } else if(tmp_buf[i] == '-')
                neg = -1;
            else if(tmp_buf[i] == '+') 
                neg = 1;
             else {
                printf("ERROR: Unknown word %s, aborting\n", tmp_buf);
                abort_();
                return;
            }

            for(; i != strlen(tmp_buf); i++) {
                if(!isdigit(tmp_buf[i])) {
                    printf("ERROR: Unknown word %s, aborting\n", tmp_buf);
                    abort_();
                    return;
                } else {
                    num += (tmp_buf[i] - '0') * ipow(sys.base, di++); 
                }
            }
            num *= neg;
            stack_push_d(num);
        } else {
            int32_t num = 0;
            if(isdigit(tmp_buf[i])) {
                num += (tmp_buf[i] - '0') * ipow(sys.base, di++); 
            } else if(tmp_buf[i] == '-')
                neg = -1;
            else if(tmp_buf[i] == '+') 
                neg = 1;
            else {
                printf("ERROR: Unknown word %s, aborting\n", tmp_buf);
                abort_();
                return;
            }

            for(; i != strlen(tmp_buf); i++) {
                if(!isdigit(tmp_buf[i])) {
                    printf("ERROR: Unknown word %s, aborting\n", tmp_buf);
                    abort_();
                    return;
                } else {
                    num += (tmp_buf[i] - '0') * ipow(sys.base, di++); 
                }
            }
            num *= neg;
            stack_push(num);
        }
    }
}
