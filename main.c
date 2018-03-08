// C imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// .h file imports
#include "stack.h"
#include "sys.h"
#include "forth/intmath.h"
#include "forth/logic.h"
#include "forth/output.h"
#include "forth/memory.h"
#include "forth/strmanip.h"
#include "forth/stackmanip.h"
#include "forth/rstack.h"
#include "forth/control.h"

struct System sys;

//Size of the FORTH system size, in words (for now, 4 MB or 2^20 words)
#define SYSTEM_SIZE 1048576

//Runs the FORTH program at func within the FORTH environment
void exec(uint32_t* func) {
    rstack_push(0);
    sys.inst = func;
    while(sys.inst) {
        uint32_t* xt_ptr = (uint32_t*)*sys.inst;
        if(sys.gloss_base < xt_ptr && xt_ptr < sys.cp) {
            rstack_push((int32_t) sys.inst);
            sys.inst = xt_ptr;
        } else {
            void(*fn)() = (void(*)()) *sys.inst;
            fn();
            if(sys.inst)
                sys.inst++;
        }
    }
}

//Adds a new empty definition to the dictionary with the
//provided name and precedence, and returns a pointer to
//the end of the new definition
uint32_t* add_def(char* name, uint8_t precedence) {
    //Set up traversal pointers
    uint32_t *new_wd = sys.cp;
    uint8_t *ccp = (uint8_t*) sys.cp;
    //Set precedence byte
    *ccp = precedence;
    ccp++;
    //Set string length byte
    uint8_t len = (uint8_t) strlen(name);
    *ccp = len;
    ccp++;
    //Copy the name into glossary memory
    memcpy(ccp, name, len);
    ccp += len;
    //Move system cp pointer
    sys.cp = (uint32_t*) ccp;
    //Set up back pointer
    *sys.cp = (uint32_t) sys.gloss_head;
    sys.cp++;
    return new_wd;
}

void add_basic_word(char* name, void(*func)(), uint8_t precedence) {
    uint32_t *new_wd = add_def(name, precedence);
    *(sys.cp) = (uint32_t) func;
    sys.cp++;
    *(sys.cp) = (uint32_t) exit_;
    sys.cp++;
    sys.gloss_head = new_wd;
}

int main() {
    // Set up the FORTH system
    sys.sys = (uint32_t*) malloc(SYSTEM_SIZE * sizeof(uint32_t));
    sys.stack = sys.sys + (SYSTEM_SIZE / 2);
    sys.stack_0 = sys.stack;
    sys.rstack = sys.sys + 3*(SYSTEM_SIZE / 4);
    sys.rstack_0 = sys.rstack;
    sys.cp = sys.sys + (SYSTEM_SIZE / 4);
    *sys.cp = 0;
    sys.gloss_head = sys.cp;
    sys.gloss_base = sys.gloss_head;
    sys.cp++;
	sys.tib = (char*) sys.stack_0 + 1;
    sys.base = 10;
    sys.inst = 0;

    //Build the glossary
    add_basic_word("CR", cr, 0);
    add_basic_word("SPACES", spaces, 0);
    add_basic_word("SPACE", space, 0);
    add_basic_word("EMIT", emit, 0);
    add_basic_word(".", print, 0);
    add_basic_word("U.", uprint, 0);
    add_basic_word("D.", dprint, 0);
    add_basic_word("U.R", urjprint, 0);
    add_basic_word("D.R", drjprint, 0);
    add_basic_word(".S", printS, 0);
    add_basic_word("TYPE", type, 0);
    add_basic_word("-TRAILING", trailing, 0);
    add_basic_word("+", add, 0);
    add_basic_word("D+", Dadd, 0);
    add_basic_word("M+", Madd, 0);
    add_basic_word("-", sub, 0);
    add_basic_word("D-", Dsub, 0);
    add_basic_word("*", mult, 0);
    add_basic_word("UM*", umult, 0);
    add_basic_word("M*", Mmult, 0);
    add_basic_word("/", divd, 0);
    add_basic_word("MOD", mod, 0);
    add_basic_word("/MOD", modDiv, 0);
    add_basic_word("UM/MOD", UmodDiv, 0);
    add_basic_word("SM/REM", SmodDiv, 0);
    add_basic_word("FM/MOD", FmodDiv, 0);
    add_basic_word("*/", multDiv, 0);
    add_basic_word("*/MOD", multDivMod, 0);
    add_basic_word("1+", add1, 0);
    add_basic_word("1-", sub1, 0);
    add_basic_word("2+", add2, 0);
    add_basic_word("2-", sub2, 0);
    add_basic_word("2*", lshift, 0);
    add_basic_word("2/", rshift, 0);
    add_basic_word("AND", and_, 0);
    add_basic_word("OR", or_, 0);
    add_basic_word("ABS", abs_, 0);
    add_basic_word("DABS", dabs, 0);
    add_basic_word("NEGATE", neg, 0);
    add_basic_word("DNEGATE", Dneg, 0);
    add_basic_word("MIN", min, 0);
    add_basic_word("DMIN", Dmin, 0);
    add_basic_word("MAX", max, 0);
    add_basic_word("DMAX", Dmax, 0);
    add_basic_word("SWAP", swap, 0);
    add_basic_word("2SWAP", swap2, 0);
    add_basic_word("DUP", dup, 0);
    add_basic_word("2DUP", dup2, 0);
    add_basic_word("?DUP", dup_if, 0);
    add_basic_word("OVER", over, 0);
    add_basic_word("2OVER", over2, 0);
    add_basic_word("ROT", rot, 0);
    add_basic_word("DROP", drop, 0);
    add_basic_word("DROP2", drop2, 0);
    add_basic_word("TUCK", tuck, 0);
    add_basic_word(">R", retPush, 0);
    add_basic_word("R>", retPop, 0);
    add_basic_word("I", retCopy, 0);
    add_basic_word("R@", retCopy, 0);
    add_basic_word("J", retCopy3, 0);
    add_basic_word("=", equals, 0);
    add_basic_word("D=", Dequals, 0);
    add_basic_word("<", lessThan, 0);
    add_basic_word("U<", UlessThan, 0);
    add_basic_word("D<", DlessThan, 0);
    add_basic_word("DU<", DUlessThan, 0);
    add_basic_word(">", greaterThan, 0);
    add_basic_word("0=", zeroEquals, 0);
    add_basic_word("D0=", DzeroEquals, 0);
    add_basic_word("0<", zeroLessThan, 0);
    add_basic_word("0>", zeroGreaterThan, 0);
    add_basic_word("!", store, 0);
    add_basic_word("2!", store2, 0);
    add_basic_word("C!", c_store, 0);
    add_basic_word("+!", plus_store, 0);
    add_basic_word("@", fetch, 0);
    add_basic_word("2@", fetch2, 0);
    add_basic_word("C@", c_fetch, 0);
    add_basic_word("?", query, 0);
    add_basic_word("CELL", cell, 0);
    add_basic_word("CELLS", cells, 0);
    add_basic_word("CELL+", cell_plus, 0);
    add_basic_word("FILL", fill, 0);
    add_basic_word("ERASE", erase, 0);
    add_basic_word("DUMP", dump, 0);
    add_basic_word("'", tick, 0);
    add_basic_word("EXECUTE", execute, 0);
    add_basic_word("?STACK", stack_q, 0);
    add_basic_word("SP@", sp_at, 0);
    add_basic_word("TIB", tib, 0);
    add_basic_word("#TIB", pound_tib, 0);
    add_basic_word("<#", bracket_pound, 0);
    add_basic_word("#>", pound_bracket, 0);
    add_basic_word("#", pound, 0);
    add_basic_word("#S", pounds, 0);
    add_basic_word("HOLD", hold, 0);
    add_basic_word("SIGN", sign, 0);
    add_basic_word("EXIT", exit_, 0);

    stack_push(3);
    stack_push(4);
    add();
    unsigned char name[2] = {1, '.'};
    stack_push((uint32_t) name);
    find();
    stack_pop(1);
    uint32_t* func = (uint32_t*)*stack_at(0);
    stack_pop(1);
    exec(func);

    free(sys.sys);
    return 0;
}
