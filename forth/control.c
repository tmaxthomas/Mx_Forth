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

char *get_substring(int(*func)(int)) {
    int i;
    for(; func(*sys.idx); sys.idx++);

    char *tmp_idx = sys.idx;
    for(i = 0; !func(*tmp_idx) && *tmp_idx != '\0'; i++) 
        tmp_idx++;

    char *tmp_buf = malloc(i + 1);
    for(int j = 0; j < i; j++) 
        tmp_buf[j] = sys.idx[j];

    tmp_buf[i] = '\0';

    sys.idx = tmp_idx;
    
    return tmp_buf;
}

int64_t ipow(int64_t base, int64_t exp) {
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
    char *buf = get_substring(isspace);
    int32_t xt = cfind(buf, NULL);
    stack_push(xt);
    free(buf);
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
    sys.inst--; //Needed to circumvent a particular bit of logic in exec() that causes shit to not work
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

void lbracket() { 
    sys.COMPILE = false;
}

void rbracket() {
    sys.COMPILE = true;
}

void colon() {
    //char *name = get_substring(isspace);    
}

void semicolon() {
    
}

//In case of emergency, burn everything to the ground and
//start over
void abort_(){ 
    stack_clear();
    rstack_clear();
    strcpy(sys.tib, "\0");
    sys.idx = sys.tib;
    sys.idx_loc = 0;
    sys.inst = sys.q_addr;
    sys.inst--;
    sys.ABORT = true;
}

int isquote(int ch) {
    return ch == '\"';
}

void abort_quote() {
    char *buf = get_substring(isquote);
    fprintf(stdout, "%s\n", buf);
    free(buf);
    abort_();
}

int64_t int64_convert(char *buf, int *err) {
    int64_t num = 0, neg = 1, di = 0, i = 0;
    if(isdigit(buf[i])) {
        num += (buf[i] - '0') * ipow((int64_t) sys.base, di++); 
    } else if(buf[i] == '-')
        neg = -1;
    else if(buf[i] == '+') 
        neg = 1;
    else {
        *err = 1;
        return 0;
    }

    for(i = 1; i != strlen(buf); i++) {
        if(!isdigit(buf[i])) {
            *err = 1;
            return 0;
        } else {
            num += (buf[i] - '0') * ipow((int64_t) sys.base, di++); 
        }
    }
    num *= neg;
    return num;
}

int32_t int32_convert(char *buf, int *err) {
    int32_t num = 0, neg = 1, di = 0, i = 0;

    if(isdigit(buf[i])) {
        num += (buf[i] - '0') * ipow((int64_t) sys.base, (int64_t) di++); 
    } else if(buf[i] == '-')
        neg = -1;
    else if(buf[i] == '+') 
        neg = 1;
    else {
        *err = 1;
        return 0;
    }

    for(i = 1; i != strlen(buf); i++) {
        if(!isdigit(buf[i])) {
            *err = 1;
            return 0;
        } else {
            num += (buf[i] - '0') * ipow((int64_t) sys.base, (int64_t) di++); 
        }
    }
    num *= neg;
    return num;
}

//Word that runs the FORTH system, including the interpreter/compiler
//Well, for now just the interpreter. But you get the point.
void quit() {
    sys.ABORT = false;

    //If QUIT was called by some means other than the normal way
    //QUIT loops
    if(sys.rstack == sys.rstack_0 || *rstack_at(0) != 0) {
        sys.OKAY = false;
        rstack_clear();
        rstack_push(0);
    }

    rstack_push((int32_t) sys.q_addr);
    
    char *buf = get_substring(isspace);
    
    //If we need to read some input, do so
    if(strlen(buf) == 0) {
        if(sys.OKAY) {
            printf("ok");
        } else {
            sys.OKAY = true;
        }
        printf("\n");
        int num_bytes = read(0, sys.tib, sys.tib_len);
        sys.tib[num_bytes - 1] = '\0'; //Chop off the trailing newline
        sys.tib[num_bytes] = '\0';
        for(int i = 0; i < strlen(sys.tib); i++) {
            if(islower(sys.tib[i])) sys.tib[i] = toupper(sys.tib[i]);
        }
        sys.idx = sys.tib;
        sys.idx_loc = 0;
        buf = get_substring(isspace);
    }
    
    if(strlen(buf) == 0) {
        free(buf);
        return;
    } 

    int precedence;
    int32_t wd = cfind(buf, &precedence);
    if(wd) {
        rstack_push(cfind(buf, NULL));
    } else {
        int err = 0;
        if(buf[strlen(buf)] == '.') {
            buf[strlen(buf)] = '\0';
            int64_t num = int64_convert(buf, &err);
            if(err) {
                fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
                free(buf);
                abort_();
                return;
            }
            stack_push_d(num);
        } else {
            int32_t num = int32_convert(buf, &err);
            if(err) {
                fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
                free(buf);
                abort_();
                return;
            }
            stack_push(num);
        }
    }
    free(buf);
}

void bye() {
    free(sys.sys);
    exit(0);
}
