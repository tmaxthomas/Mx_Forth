#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../sys.h"
#include "../stack.h"
#include "../defs.h"
#include "control.h"


/* HELPER FUNCTIONS */

// Runs the FORTH program at func within the FORTH environment
void exec(uint32_t * func) {
    rstack_push(0);
    sys.inst = func;
    // Main program loop - run until the instruction pointer is NULL
    while(sys.inst) {
        uint32_t *xt_ptr = sys_addr(*sys.inst);
        // If the top of the rstack pointed somewhere in the glossary, it's a FORTH word. Call it.
        if (sys.gloss_base < xt_ptr && xt_ptr < sys.cp) {
            rstack_push(forth_addr(sys.inst + 1));
            sys.inst = xt_ptr;
        // If the top of the stack contained a function table address,
        // run the function and increment.
        } else if (*sys.inst < ft_size) {
            func_table[*sys.inst]();
            if (sys.inst) sys.inst++;
        } else {
            fprintf(stderr, "ERROR: Invalid execution address, aborting\n");
            abort_();
        }
    }
}

// Finds substrings delimited by characters that return 1 when fed to func() in a malloc-ed buffer
char *get_substring(int(*func)(int)) {
    for (; func(*sys.idx) && sys.idx_loc < sys.idx_len; sys.idx++, sys.idx_loc++);

    char *tmp_idx = sys.idx;
    uint32_t tmp_loc = sys.idx_loc, i;
    for (i = 0; !func(*tmp_idx) && tmp_loc < sys.idx_len; i++) {
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

// 64-bit integer pow() function
int64_t ipow(int64_t base, int64_t exp) {
    int64_t ret = 1;
    for (int i = 0; i < exp; i++) {
        ret *= base;
    }

    return ret;
}

// Compares two counted (FORTH) strings for equality
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

// Returns a system pointer to the start of the code section for func
uint32_t *get_xt(uint32_t * func) {
    uint8_t len = *(((uint8_t *) func) + 1);
    uintptr_t xt = ((uintptr_t) func) + len + 6;
    return (uint32_t *) xt;
}

// Finds a c string in the dictionary, if it exists, returning a FORTH address with
// the xt corresponding to the string
uint32_t cfind(char *str, int *precedence) {
    char *fstr = (char *) sys.cp + 256;
    fstr[0] = strlen(str);
    memcpy(fstr + 1, str, strlen(str));
    stack_push(forth_addr((uint32_t *) fstr));
    find();
    if (*stack_at(0) == 0) {
        stack_pop(2);
        return 0;
    } else {
        if (precedence) {
            *precedence = *stack_at(0);
        }
        int32_t ret = *stack_at(1);
        stack_pop(2);
        return ret;
    }
}

// Case-insensitive base-compliant digit converter
int to_num(int c) {
    if (c >= 'a' && c <= 'z') {
        c -= 87;
    } else if (c >= 'A' && c <= 'Z') {
        c -= 55;
    } else if (isdigit(c)) {
        c -= '0';
    }
    return (c < (int) sys.base) ? c : -1;
}

// Converts the string in buf to a signed 64-bit integer
int64_t int64_convert(char *buf, int *err) {
    int64_t num = 0, neg = 1;

    if (buf[0] == '-') {
        neg = -1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    } else if (buf[0] == '+') {
        neg = 1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    }

    for (int i = strlen(buf); i > 0; i--) {
        int n;
        if ((n = to_num(buf[i - 1])) == -1) {
            *err = 1;
            return 0;
        } else {
            num += n * ipow((int64_t) sys.base, (int64_t) (strlen(buf) - i));
        }
    }
    num *= neg;
    return num;
}

// Converts the string in buf to a signed 32-bit integer
int32_t int32_convert(char *buf, int *err) {
    int32_t num = 0, neg = 1;

    if (buf[0] == '-') {
        neg = -1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    } else if (buf[0] == '+') {
        neg = 1;
        memmove(buf, buf + 1, strlen(buf) + 1);
    }

    for (int i = strlen(buf); i > 0; i--) {
        int n;
        if ((n = to_num(buf[i - 1])) == -1) {
            *err = 1;
            return 0;
        } else {
            num += n * ipow((int64_t) sys.base, (int64_t) (strlen(buf) - i));
        }
    }
    num *= neg;
    return num;
}

// Interprets the word/number contained within buf
void interpret(char *buf) {
    int precedence;
    int32_t wd = cfind(buf, &precedence);

    if (wd) {
        if (!*sys.COMPILE || precedence == -1) {
            rstack_push(wd);
        } else {
            uint32_t *wd_ptr = sys_addr(wd);

            if (!wd_ptr[1] && !wd_ptr[2]) {
                *sys.cp = *wd_ptr;
            } else {
                *sys.cp = (uint32_t) wd;
            }
            sys.cp++;
        }
    } else {
        int err = 0;

        // Need two separate cases, one for 32-bit numbers, one for 64-bit
        if (buf[strlen(buf)] == '.') {
            buf[strlen(buf)] = '\0';
            int64_t num = int64_convert(buf, &err);
            // If this wasn't actually a number
            if (err) {
                fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
                abort_();
                return;
            }
            // If we're in interpretation mode
            if (!*sys.COMPILE) {
                stack_push_d(num);
            // If we're in compilation mode
            } else {
                *(sys.cp) = DNUM_RUNTIME_ADDR;
                sys.cp++;
                *(int64_t*) (sys.cp) = num;
                sys.cp += 2;
            }
        } else {
            int32_t num = int32_convert(buf, &err);
            // If this wasn't actually a number
            if (err) {
                fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
                abort_();
                return;
            }
            // If we're in interpretation mode
            if (!*sys.COMPILE) {
                stack_push(num);
            // If we're in compilation mode
            } else {
                *(sys.cp) = NUM_RUNTIME_ADDR;
                sys.cp++;
                *(int32_t *) sys.cp = num;
                sys.cp++;
            }
        }
    }
}

/* FORTH FUNCTIONS */

void find() {
    uint8_t *name = (uint8_t *) sys_addr(*stack_at(0));
    stack_pop(1);
    uint32_t *gloss_loc = sys.gloss_head;
    while (*gloss_loc) {
        uint8_t *ccp = (uint8_t *) gloss_loc;
        if (str_eq(ccp + 1, name)) {
            stack_push(forth_addr(get_xt(gloss_loc)));
            if (*ccp == 0) { // Not immediate
                stack_push(1);
            } else { // Immediate
                stack_push(-1);
            }
            return;
        } else {
            uint8_t len = *(ccp + 1);
            // Locate back pointer and assign dereferenced value to gloss_loc
            uint32_t *bp = (uint32_t *) (((uintptr_t) gloss_loc) + len + 2);
            gloss_loc = sys_addr(*bp);
        }
    }
    stack_push(forth_addr((uint32_t *) name));
    stack_push(0);
}

void tick() {
    char *buf = get_substring(isspace);
    for (uint32_t i = 0; i < strlen(buf); i++) {
        buf[i] = toupper(buf[i]);
    }
    int32_t xt = cfind(buf, NULL);
    if (!xt) {
        fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
        free(buf);
        abort_();
        return;
    }
    stack_push(xt);
    free(buf);
}

void bracket_tick_bracket() {
    char *buf = get_substring(isspace);
    for (uint32_t i = 0; i < strlen(buf); i++) {
        buf[i] = toupper(buf[i]);
    }

    int32_t xt = cfind(buf, NULL);
    if (!xt) {
        fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
        free(buf);
        abort_();
        return;
    }

    *sys.cp = NUM_RUNTIME_ADDR;
    sys.cp++;
    *sys.cp = xt;
    sys.cp++;
    free(buf);
}

void bracket_char_bracket() {
    char *buf = get_substring(isspace);
    *sys.cp = NUM_RUNTIME_ADDR;
    sys.cp++;
    *sys.cp = buf[0];
    sys.cp++;
    free(buf);
}

void execute() {
    uint32_t xt = *(uint32_t *) stack_at(0);
    stack_pop(1);
    uint32_t *xt_addr = sys_addr(xt);
    if (sys.gloss_base < xt_addr && xt_addr < sys.cp) {
        rstack_push(forth_addr(sys.inst + 1));
        sys.inst = sys_addr(xt);
        sys.inst--;
    } else {
        fprintf(stderr, "ERROR: invalid xt passed to EXECUTE, aborting\n");
        abort_();
    }
}

void exit_() {
    sys.inst = sys_addr(*rstack_at(0));
    rstack_pop(1);
    sys.inst--;
}

void if_() {
    *sys.cp = COND_JUMP_ADDR;
    sys.cp++;
    stack_push(forth_addr(sys.cp));
    sys.cp++;
}

void else_() {
    uint32_t *loc = sys_addr(*stack_at(0));
    stack_pop(1);

    *sys.cp = JUMP_ADDR;
    sys.cp++;
    stack_push(forth_addr(sys.cp));
    sys.cp++;

    *loc = forth_addr(sys.cp);
}

void then(){
    uint32_t *loc = sys_addr(*stack_at(0));
    stack_pop(1);
    *loc = forth_addr(sys.cp);
}

void do_() {
    *sys.cp = DO_RUNTIME_ADDR;
    sys.cp++;
    stack_push(forth_addr(sys.cp));
    stack_push(0);
}

void do_runtime() {
    int32_t i = *(int32_t *) stack_at(0),
            n = *(int32_t *) stack_at(1);
    stack_pop(2);
    rstack_push(n);
    rstack_push(i);
}

void loop() {
    // Handle all the LEAVE state
    while (*stack_at(0)) {
        uint32_t *leave_addr = sys_addr(*stack_at(0));
        *leave_addr = forth_addr(sys.cp + 2);
        stack_pop(1);
    }
    stack_pop(1);

    uint32_t do_addr = *stack_at(0);
    stack_pop(1);
    *sys.cp = LOOP_RUNTIME_ADDR;
    sys.cp++;
    *sys.cp = do_addr;
    sys.cp++;
}

void loop_runtime() {
    int32_t *i = (int32_t *) rstack_at(0),
            n = *(int32_t *) rstack_at(1);

    (*i)++;
    if (*i == n) {
        rstack_pop(2);
        sys.inst++;
    } else {
        jump();
    }
}

void plus_loop() {
    // Handle all the LEAVE state
    while (*stack_at(0)) {
        uint32_t *leave_addr = sys_addr(*stack_at(0));
        *leave_addr = forth_addr(sys.cp + 2);
        stack_pop(1);
    }
    stack_pop(1);

    uint32_t do_addr = *stack_at(0);
    stack_pop(1);
    *sys.cp = PLUS_LOOP_RUNTIME_ADDR;
    sys.cp++;
    *sys.cp = do_addr;
    sys.cp++;
}

void plus_loop_runtime() {
    int32_t *i = (int32_t *) rstack_at(0),
            n = *(int32_t *) rstack_at(1),
            k = *(int32_t *) stack_at(0);

    stack_pop(1);

    *i += k;
    if ((k > 0 && *i >= n) || (k <= 0 && *i < n)) {
        rstack_pop(2);
        sys.inst++;
    } else {
        jump();
    }
}

void begin() {
    stack_push(forth_addr(sys.cp));
}

void until() {
    uint32_t jmp_addr = *stack_at(0);
    stack_pop(1);
    *sys.cp = COND_JUMP_ADDR;
    sys.cp++;
    *sys.cp = jmp_addr;
    sys.cp++;
}

void while_() {
    *sys.cp = COND_JUMP_ADDR;
    sys.cp++;
    stack_push(forth_addr(sys.cp));
    sys.cp++;
}

void repeat() {
    uint32_t *while_addr = sys_addr(*stack_at(0)),
             begin_addr = *stack_at(1);

    stack_pop(2);

    *sys.cp = JUMP_ADDR;
    sys.cp++;
    *sys.cp = begin_addr;
    sys.cp++;
    *while_addr = forth_addr(sys.cp);
}

void jump() {
    sys.inst = sys_addr(*(sys.inst + 1));
    sys.inst--;
}

void cond_jump() {
    uint32_t flag = *stack_at(0);
    stack_pop(1);
    if (!flag) {
        jump();
    } else {
        sys.inst++;
    }
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
    *sys.COMPILE = false;
}

void rbracket() {
    *sys.COMPILE = true;
}

void colon() {
    char *name = get_substring(isspace);
    sys.curr_def = add_def(name, 0);
    stack_push(forth_addr(sys.curr_def));
    free(name);
    rbracket();
}

void semicolon() {
    *sys.cp = EXIT_ADDR;
    sys.cp++;
    *sys.cp = 0;
    sys.cp++;
    uint32_t *new_wd = sys_addr(*stack_at(0));
    stack_pop(1);
    sys.gloss_head = new_wd;
    sys.old_cp = sys.cp;
    lbracket();
}

// In case of emergency, burn everything to the ground and start over
void abort_() {
    // Empty the stack and rstack
    stack_clear();
    rstack_clear();
    // Clear the terminal input buffer
    strcpy(sys.tib, "");
    sys.idx = sys.tib;
    sys.idx_loc = 0;
    // Clean up whatever garbage might be at the end of the glossary
    // (in case we aborted while compiling a definition)
    sys.cp = sys.old_cp;
    // Reset the instruction pointer
    sys.inst = sys.q_addr-1;
    sys.ABORT = true;
    *sys.COMPILE = false;
}

int isquote(int ch) {
    return ch == '\"';
}

void abort_quote() {
    char *buf = get_substring(isquote);
    *sys.cp = ABORT_QUOTE_RUNTIME_ADDR;
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
        // Write the error string to stderr and then abort
        sys.inst++;
        char *c = (char *) sys.inst;
        write(STDERR_FILENO, c+1, *c);
        abort_();
    } else {
        // Otherwise, jump over the error string and carry on
        sys.inst++;
        char *c = (char *) sys.inst;
        sys.inst += ((*c + 1) / 4);
        if ((*c + 1) % 4 == 0) {
            sys.inst--;
        }
    }
    return;
}


// Word that runs the FORTH system, including the interpreter/compiler
void quit() {
    // We're not aborting anymore
    sys.ABORT = false;

    // If QUIT was called by some means other than the normal way
    // QUIT loops
    if (sys.rstack == sys.rstack_0 || *rstack_at(0) != 0) {
        sys.OKAY = false;
        rstack_clear();
        rstack_push(0);
    }

    char *buf = get_substring(isspace);
    for (uint32_t i = 0; i < strlen(buf); i++) {
        if (islower(buf[i])) buf[i] = toupper(buf[i]);
    }

    // If we need to get some more input, do so
    if (strlen(buf) == 0) {
        if (sys.source_id == 0) {
            free(buf);
            // If "ok" needs to be printed, print it
            // otherwise establish that it'll need to be printed
            if (sys.OKAY) {
                printf("ok\n");
            } else {
                sys.OKAY = true;
            }
            
            // Get us some input using Readline
            char *tmpbuf = readline(NULL);
            sys.idx_len = strlen(tmpbuf);
            // If the input is too long, freak out
            if (sys.idx_len >= sys.tib_len) {
                fprintf(stderr, "ERROR: Inputted line exceeds terminal input buffer size, aborting\n");
                free(tmpbuf);
                return;
            }

            memcpy(sys.tib, tmpbuf, sys.idx_len+1);

            sys.idx = sys.tib;
            sys.idx_loc = 0;
        // If quit() was run from within evaluate()
        } else {
            free(buf);
            return;
        }

        buf = get_substring(isspace);
        for (uint32_t i = 0; i < strlen(buf); i++) {
            if (islower(buf[i])) buf[i] = toupper(buf[i]);
        }
    }

    rstack_push(sys.q_fth_addr);

    interpret(buf);
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
    uintptr_t dict_ptr_val = 1 + (uintptr_t) sys.gloss_head;
    uint8_t len = *(uint8_t *) dict_ptr_val;
    dict_ptr_val += len + 5;
    uint32_t *dict_ptr = (uint32_t *) dict_ptr_val;
    *dict_ptr = DOES_RUNTIME_ADDR;
    dict_ptr++;
    memmove(dict_ptr + 1, dict_ptr, sys_util.alloc);
    *dict_ptr = forth_addr(sys.inst + 1);
    exit_();
}

void does_runtime() {
    stack_push(forth_addr(sys.inst + 2));
    sys.inst = sys_addr(*(sys.inst + 1));
    sys.inst--;
}

void evaluate() {
    // Store the current input source, whatever it may be, and set
    // the base of the rstack to make a new stack frame
    rstack_push(forth_addr((uint32_t *) sys.idx));
    rstack_push(sys.idx_len);
    rstack_push(sys.idx_loc);
    rstack_push(sys.source_id);
    rstack_push(forth_addr(sys.inst));
    rstack_push(forth_addr(sys.rstack_0));
    sys.rstack_0 = sys.rstack;
    sys.idx_len = *stack_at(0);
    sys.idx = (char *) sys_addr(*stack_at(1));
    stack_pop(2);
    sys.idx_loc = 0;
    sys.source_id = -1;

    // Now spin up a new instance of the FORTH system
    exec(sys.q_addr);

    // Finally, pop the stack frame we made
    sys.rstack = sys.rstack_0;
    sys.rstack_0 = sys_addr(*rstack_at(0));
    sys.inst = sys_addr(*rstack_at(1));
    sys.source_id = *rstack_at(2);
    sys.idx_loc = *rstack_at(3);
    sys.idx_len = *rstack_at(4);
    sys.idx = (char *) sys_addr(*rstack_at(5));
    rstack_pop(6);
}

void immediate() {
    uint8_t *precedence = (uint8_t *) sys.gloss_head;
    *precedence = 1;
}

void literal() {
    uint32_t num = *stack_at(0);
    stack_pop(1);
    *(sys.cp) = NUM_RUNTIME_ADDR;
    sys.cp++;
    *(int32_t *) (sys.cp) = num;
    sys.cp++;
}

void postpone() {
    char *buf = get_substring(isspace);
    int32_t wd = cfind(buf, NULL);
    if (wd) {
        // Some necessary optimizations, needed to make some stuff work
        if (*(sys_addr(wd) + 1) == EXIT_ADDR) {
            *sys.cp = *sys_addr(wd);
        } else {
            *sys.cp = (uint32_t) wd;
        }
        sys.cp++;
        free(buf);
    } else {
        fprintf(stderr, "ERROR: Unknown word %s, aborting\n", buf);
        free(buf);
        abort_();
    }
}

void state() {
    stack_push(forth_addr(sys.COMPILE));
}

void unloop() {
    rstack_pop(2);
}

void leave() {
    *sys.cp = UNLOOP_ADDR;
    sys.cp++;
    *sys.cp = JUMP_ADDR;
    sys.cp++;
    int i;
    for (i = 0; *stack_at(i); i++);
    memmove(sys.stack - 1, sys.stack, i * 4);
    sys.stack--;
    *stack_at(i) = forth_addr(sys.cp);
    sys.cp++;
}

void recurse() {
    *(sys.cp) = forth_addr(get_xt(sys.curr_def));
    sys.cp++;
}
