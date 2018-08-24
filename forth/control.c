#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../sys.h"
#include "../stack.h"
#include "control.h"


/* HELPER FUNCTIONS */

//Runs the FORTH program at func within the FORTH environment
void exec(uint32_t* func) {
    rstack_push(0);
    sys.inst = func;
    while(sys.inst) {
        uint32_t* xt_ptr = (uint32_t*)*sys.inst;
        if(sys.gloss_base < xt_ptr && xt_ptr < sys.cp) {
            rstack_push((int32_t) (sys.inst + 1));
            sys.inst = xt_ptr;
        } else {
            void(*fn)() = (void(*)()) *sys.inst;
            fn();
            if(sys.inst)
                sys.inst++;
        }
    }
}


char *get_substring(int(*func)(int)) {
    for(; func(*sys.idx) && sys.idx_loc < sys.idx_len; sys.idx++) {
        sys.idx_loc++;
    }

    char *tmp_idx = sys.idx;
    int tmp_loc = sys.idx_loc, i;
    for(i = 0; !func(*tmp_idx) && tmp_loc < sys.idx_len; i++) {
        tmp_idx++;
        tmp_loc++;
    }

    char *tmp_buf = malloc(i + 1);
    memcpy(tmp_buf, sys.idx, i);

    tmp_buf[i] = '\0';

    sys.idx = tmp_idx + 1;
    sys.idx_loc = tmp_loc + 1; 
    
    return tmp_buf;
}

int64_t ipow(int64_t base, int64_t exp) {
    int64_t ret = 1;
    for(int i = 0; i < exp; i++)
        ret *= base;
    
    return ret;
}

// Compares two counted strings for equality
bool str_eq(uint8_t* c1, uint8_t* c2) {
    uint8_t len = 0, *buf = NULL;
    if (*c1 <= *c2) {
        len = *c2;
        buf = malloc(len + 1);
        memset(buf, 0, len + 1);
        memcpy(buf, c1, *c1 + 1);
        c1 = buf;
    } else {
        len = *c1;
        buf = malloc(len + 1);
        memset(buf, 0, len + 1); 
        memcpy(buf, c2, *c2 + 1);
        c2 = buf;
    }
    int result;
    result = strncmp((char *) (c1 + 1), (char *) (c2 + 1), len) == 0;
    free(buf);
    return result;
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
    for(int i = 0; i < strlen(buf); i++) {
        buf[i] = toupper(buf[i]);
    }
    int32_t xt = cfind(buf, NULL);
    stack_push(xt);
    free(buf);
}

// ( addr -- )
// Executes the word pointed to by addr by moving the instruction pointer
void execute() {
    uint32_t* xt_ptr = *(uint32_t **) stack_at(0);
    stack_pop(1);
    rstack_push((int32_t) (sys.inst + 1));
    rstack_push((int32_t) (xt_ptr));
}

// ( -- )
//Sets the instruction pointer to the top of the rstack, then pops the rstack
void exit_() {
    sys.inst = (uint32_t*)*rstack_at(0);
    rstack_pop(1);
    sys.inst--; //Needed to circumvent a particular bit of logic in exec() that causes shit to not work
}

void if_() {
    *sys.cp = (uint32_t) cond_jump;
    sys.cp++;
    stack_push((int32_t) sys.cp);
    sys.cp++;
}

void else_() { 
    uint32_t **loc = *(uint32_t ***)stack_at(0);
    stack_pop(1);

    *sys.cp = (uint32_t) jump;
    sys.cp++;
    stack_push((int32_t) sys.cp);
    sys.cp++;

    *loc = sys.cp;
}

void then(){
    uint32_t **loc = *(uint32_t ***)stack_at(0);
    stack_pop(1);
    *loc = sys.cp;
}

void do_() { 
    *sys.cp = (uint32_t) do_runtime;
    sys.cp++;
    stack_push((int32_t) sys.cp);
}

void do_runtime() {
    int32_t i = *(int32_t *) stack_at(0),
            n = *(int32_t *) stack_at(1);
    stack_pop(2);
    rstack_push(n);
    rstack_push(i);
}

void loop() {
    uint32_t addr = *stack_at(0); 
    stack_pop(1);
    *sys.cp = (uint32_t) loop_runtime;
    sys.cp++;
    *sys.cp = addr;
    sys.cp++;
}

void loop_runtime() {
    int32_t *i = (int32_t *) rstack_at(0),
            n = *(int32_t *) rstack_at(1);
    
    (*i)++;
    if(*i == n) {
        rstack_pop(2);
        sys.inst++;
    } else {
        jump();
    } 
}

void plus_loop() {
    uint32_t addr = *stack_at(0); 
    stack_pop(1);
    *sys.cp = (uint32_t) plus_loop_runtime;
    sys.cp++;
    *sys.cp = addr;
    sys.cp++;
}

void plus_loop_runtime() {
    int32_t *i = (int32_t *) rstack_at(0),
            n = *(int32_t *) rstack_at(1),
            k = *(int32_t *) stack_at(0);
    
    stack_pop(1);
    
    *i += k;
    if((k > 0 && *i >= n) || (k <= 0 && *i < n)) {
        rstack_pop(2);
        sys.inst++;
    } else {
        jump();
    }
}

void begin() { 
    stack_push((int32_t) sys.cp);
}

void until() {
    uint32_t jmp_addr = *stack_at(0);
    stack_pop(1);
    *sys.cp = (uint32_t) cond_jump;
    sys.cp++;
    *sys.cp = jmp_addr;
    sys.cp++; 
}

void while_() { 
    *sys.cp = (uint32_t) cond_jump;
    sys.cp++;
    stack_push((int32_t) sys.cp);
    sys.cp++;
}

void repeat() {
    uint32_t *while_addr = *(uint32_t **) stack_at(0),
             begin_addr = *stack_at(1);

    stack_pop(2);
    
    *sys.cp = (uint32_t) jump;
    sys.cp++;
    *sys.cp = begin_addr;
    sys.cp++;
    *while_addr = (uint32_t) sys.cp;
}

void jump() { 
    sys.inst = *(uint32_t **) (sys.inst + 1);
    sys.inst--;
}

void cond_jump() { 
    uint32_t flag = *stack_at(0);
    stack_pop(1);
    if(!flag) 
        jump();
    else
        sys.inst++;
}

void num_runtime() {
    sys.inst++;
    stack_push(*sys.inst);
}

void dnum_runtime() {
    sys.inst++;
    stack_push_d(*(int64_t *) sys.inst);
    sys.inst++;
}

void lbracket() { 
    sys.COMPILE = false;
}

void rbracket() {
    sys.COMPILE = true;
}

void colon() {
    char *name = get_substring(isspace);
    uint32_t *new_wd = add_def(name, 0);
    stack_push((int32_t) new_wd);
    free(name);
    rbracket(); 
}

void semicolon() {
    *(sys.cp) = (uint32_t) exit_;
    sys.cp++;
    uint32_t *new_wd = *(uint32_t**)stack_at(0);
    stack_pop(1);
    sys.gloss_head = new_wd; 
    sys.old_cp = sys.cp;
    lbracket();
}

//In case of emergency, burn everything to the ground and start over
void abort_() { 
    stack_clear();
    rstack_clear();
    strcpy(sys.tib, "");
    sys.idx = sys.tib;
    sys.idx_loc = 0;
    sys.cp = sys.old_cp;
    sys.inst = sys.q_addr;
    sys.inst--;
    sys.ABORT = true;
    sys.COMPILE = false;
}

int isquote(int ch) {
    return ch == '\"';
}

void abort_quote() {
    char *buf = get_substring(isquote);
    *sys.cp = (uint32_t) abort_quote_runtime;
    sys.cp++;
    char *ccp = (char *) sys.cp;
    *ccp = strlen(buf);
    memcpy(ccp + 1, buf, *ccp);
    int count = *ccp + 1;
    sys.cp += count / 4;
    if (count % 4 != 0) sys.cp++;
    free(buf);
}

void abort_quote_runtime() {
    uint32_t flag = *stack_at(0);
    stack_pop(1);
    if (flag) {
        sys.inst++;
        char *c = (char *) sys.inst;
        char *buf = malloc(*c + 1);
        memcpy(buf, c + 1, *c);
        buf[(int) *c] = '\0';
        printf("%s", buf);
        free(buf);
        abort_();
    } else {
        sys.inst++;
        char *c = (char *) sys.inst;
        sys.inst += ((*c + 1) / 4);
        if((*c + 1) % 4 == 0)
            sys.inst--;
    }
    return;
}

void swp(char *c1, char *c2) {
    char a = *c1, b = *c2;
    *c1 = b, *c2 = a;
}

//Case-insensitive base-compliant digit converter
int to_num(int c) {
    if(c >= 'a' && c <= 'z') {
        c -= 87;
        if(c < sys.base) 
            return c;
        else 
            return -1;
    } else if(c >= 'A' && c <= 'Z') {
        c -= 55;
        if(c < sys.base)
            return c;
        else
            return -1;
    } else if(isdigit(c)) {
        c -= '0';
        if(c < sys.base)
            return c;
    }

    return -1; 
}

int64_t int64_convert(char *buf, int *err) {
    int64_t num = 0, neg = 1;

    if(buf[0] == '-') {
        neg = -1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    } else if(buf[0] == '+') {
        neg = 1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    } 

    for(int i = strlen(buf); i > 0; i--) {
        int n;
        if((n = to_num(buf[i - 1])) == -1) {
            *err = 1;
            return 0;
        } else {
            num += n * ipow((int64_t) sys.base, (int64_t) (strlen(buf) - i));
        }
    }
    num *= neg;
    return num;
}

int32_t int32_convert(char *buf, int *err) {
    int32_t num = 0, neg = 1;

    if(buf[0] == '-') {
        neg = -1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    } else if(buf[0] == '+') {
        neg = 1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    } 

    for(int i = strlen(buf); i > 0; i--) {
        int n;
        if((n = to_num(buf[i - 1])) == -1) {
            *err = 1;
            return 0;
        } else {
            num += n * ipow((int64_t) sys.base, (int64_t) (strlen(buf) - i));
        }
    }
    num *= neg;
    return num;
}

// Word that runs the FORTH system, including the interpreter/compiler
void quit() {
    // We're not aborting anymore
    sys.ABORT = false;

    // If QUIT was called by some means other than the normal way
    // QUIT loops
    if(sys.rstack == sys.rstack_0 || *rstack_at(0) != 0) {
        sys.OKAY = false;
        rstack_clear();
        rstack_push(0);
    }
    
    char *buf = get_substring(isspace);
    for(int i = 0; i < strlen(buf); i++) {
        if(islower(buf[i])) buf[i] = toupper(buf[i]);
    }
    
    // If we need to get some more input, do so
    if(strlen(buf) == 0) {
        if (sys.source_id == 0) {
            free(buf);
            if(sys.OKAY) {
                printf("ok");
            } else {
                sys.OKAY = true;
            }
            printf("\n");
            int num_bytes = read(0, sys.tib, sys.tib_len);
            sys.tib[num_bytes - 1] = '\0'; // Chop off the trailing newline
            // sys.tib[num_bytes] = '\0';
            sys.idx_len = num_bytes - 1;
            
            sys.idx = sys.tib;
            sys.idx_loc = 0;
        } else { // If quit() was run from within evaluate()
            free(buf);
            return;
        }

        buf = get_substring(isspace);
        for(int i = 0; i < strlen(buf); i++) {
            if(islower(buf[i])) buf[i] = toupper(buf[i]);
        }
    }
    
    rstack_push((int32_t) sys.q_addr);
    
    int precedence;
    int32_t wd = cfind(buf, &precedence);

    if(wd) {
        if(!sys.COMPILE || precedence == -1) {
            rstack_push(wd);
        } else {
            // Some necessary optimizations, needed to make some stuff work
            if(*(((uint32_t *) wd) + 1) == (uint32_t) exit_) {
                *(sys.cp) = *(uint32_t *) wd;
            } else {
                *(sys.cp) = (uint32_t) wd; 
            }
            sys.cp++;

        }
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
            if(!sys.COMPILE) {
                stack_push_d(num);
            } else {    
                *(sys.cp) = (uint32_t) dnum_runtime;
                sys.cp++;
                *(int64_t*) (sys.cp) = num;
                sys.cp += 2;
            }
        } else {
            int32_t num = int32_convert(buf, &err);
            if(err) {
                fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
                free(buf);
                abort_();
                return;
            }
            if(!sys.COMPILE) {
                stack_push(num);
            } else {
                *(sys.cp) = (uint32_t) num_runtime;
                sys.cp++;
                *(int32_t*) (sys.cp) = num;
                sys.cp++;
            }
        }
    }
    free(buf);
}

void bye() {
    free(sys.sys);
    exit(0);
}

int is_paren(int ch) {
    return ch == ')';
}

void paren() {
    char *buf = get_substring(is_paren);
    free(buf);
}

void does() {
    uint32_t dict_ptr_val = 1 + (uint32_t) sys.gloss_head;
    uint8_t len = *(uint8_t *) dict_ptr_val;
    dict_ptr_val += len + 5;
    uint32_t *dict_ptr = (uint32_t *) dict_ptr_val;
    *dict_ptr = (uint32_t) does_runtime;
    dict_ptr++;
    memmove(dict_ptr + 1, dict_ptr, sys_util.alloc);
    *dict_ptr = (uint32_t) (sys.inst + 1);
    exit_();
}

void does_runtime() {
    stack_push((int32_t) (sys.inst + 2));
    sys.inst = *(uint32_t **) (sys.inst + 1);
    sys.inst--;    
}

void evaluate() {
    // Store the current input source, whatever it may be, and set
    // the base of the rstack to make a new stack frame
    rstack_push((int32_t) sys.idx);
    rstack_push((int32_t) sys.idx_len);
    rstack_push((int32_t) sys.idx_loc);
    rstack_push(sys.source_id);
    rstack_push((int32_t) sys.inst);
    rstack_push((int32_t) sys.rstack_0);
    sys.rstack_0 = sys.rstack;
    sys.idx_len = *stack_at(0);
    sys.idx = *(char **) stack_at(1);
    stack_pop(2);
    sys.idx_loc = 0; 
    sys.source_id = -1;
    
    // Now spin up a new instance of the FORTH system
    exec(sys.q_addr);

    // Finally, pop the stack frame we made
    sys.rstack = sys.rstack_0;
    sys.rstack_0 = *(uint32_t **) sys.rstack;
    sys.inst = *(uint32_t **) rstack_at(1);
    sys.source_id = *rstack_at(2);
    sys.idx_loc = *rstack_at(3);
    sys.idx_len = *rstack_at(4);
    sys.idx = *(char **) rstack_at(5);
    rstack_pop(6);
}

void immediate() {
    uint8_t *precedence = (uint8_t *) sys.gloss_head;
    *precedence = 1;
}
