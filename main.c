// C imports
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

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
#include "forth/stackmanip.h"
#include "forth/stackmanip.h"

struct System sys;

//Size of the FORTH system size, in words (for now, 4 MB)
#define SYSTEM_SIZE 1048576

//Utility macro for getting char-delimited substrings of C strings
#define GetSubstring(boolean) tmp_idx = sys.idx; for (i = 0; !(boolean); i++) tmp_idx++; \
                           tmp_buf = (char *) malloc(i + 1); for (size_t j = 0; j < i; j++) tmp_buf[j] = sys.idx[j]; \
                           tmp_buf[i] = 0; sys.idx = tmp_idx; if (*sys.idx) sys.idx++

//Reflective words
void tick();
void execute();

//Misc. words
void page();
void quit();
void exit();
void abort_();
void stack_q();

//Compares two counted strings for equality
bool str_eq(uint8_t* c1, uint8_t* c2) {
    for(uint8_t i = 0; i < *c1; i++)
        if(c1[i] != c2[i])
            return false;
    return true;
}

// ( c-addr -- c-addr 0 | xt 1 | xt -1 )
//Finds words in the glossary
void find() {
    uint8_t* name = (uint8_t*)*stack_at(0);
    stack_pop(1);
    uint32_t* gloss_loc = sys.gloss_head;
    while(*gloss_loc) {
        uint8_t* ccp = (uint8_t*) gloss_loc;
        if(str_eq(ccp + 1, name)) {

        } else {
            //Increment gloss_loc by the ceiling-divided name length + 2, deference,
            //and assign the dereferenced value back to gloss_loc
            gloss_loc = (uint32_t*)*(gloss_loc + ((len + 2) / 4) + (len % 4 != 0));
        }
    }
    stack_push(name);
    stack_push(0);
}

//Returns a pointer to a uint32_t that contains the xt for func
uint32_t* get_xt(uint32_t* func) {
    uint8_t len = *(((uint8_t*) func) + 1);
    return func + ((len + 2) / 4) + (len % 4 != 0) + 1;
}

/*

//Safely deletes a word, preventing segfaults and memory leaks
void delete_word(Function* word) {
    std::queue<Function*> to_delete;
    std::unordered_set<Function*> deleted;
    to_delete.push(word);
    while(!to_delete.empty()) {
        Function* next = to_delete.front();
        to_delete.pop();
        if(next)
            for(int32_t i = 0; i < next->size; i++)
                if(deleted.find(next->next[i]) == deleted.end())
                    to_delete.push(next->next[i]);
        delete next;
        deleted.insert(next);
    }
}

//FORGET word for dictionary cleanup
void forget() {
    char *tmp_buf, *tmp_idx;
    size_t i;
    GetSubstring(isspace(*tmp_idx));
    std::string name(tmp_buf);
    free(tmp_buf);

    auto itr = glossary.begin();
    for(; itr != glossary.end(); itr++)
        if((*itr).first == name) break;

    if(itr != glossary.end()) {
        for(auto itr2 = itr; itr2 != glossary.end(); itr2++)
            delete_word((*itr2).second);
        glossary.erase(itr, glossary.end());
    }
}


*/


// ( -- addr )
// Pushes the exectuion address of the next word in the input stream onto the stack
void tick() {
    char *tmp_buf, *tmp_idx;
    size_t i;
    GetSubstring(isspace(*tmp_idx));
    uint32_t* xt = get_xt(find(tmp_buf));
    stack_push((uint32_t) xt);
}

// ( addr -- )
// Executes the word pointed to by addr
void execute() {
    uint32_t* xt_ptr = stack_at(0);
    stack_pop(1);
    sys.inst = xt_ptr;
    void(*xt)() = (void(*)()) *xt_ptr;
    xt();
}

// ( -- )
//Clears the screen
void page() {
    system("clear");
}

// ( -- )
//Aborts the program
void abort_() {

}

// ( -- )
//Sets up the interpreter to not print ok, among other things
void quit() {

}

// ( -- f )
// Pushes true if the stack is empty, pushes false otherwise
void stack_q() {
    if(sys.stack == sys.stack_0) stack_push((int32_t)0xffffffff);
    else stack_push(0x00000000);

}

// ( -- )
// Compiled only-used to trigger termination of word execution
void exit_() {

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
    //Maintain alignment
    uint32_t ccp_val = (uint32_t) ccp;
    ccp_val += ccp_val % 4;
    //Move system cp pointer
    sys.cp = (uint32_t*) ccp_val;
    sys.cp++;
    //Set up back pointer
    *sys.cp = (uint32_t) sys.gloss_head;
    sys.cp++;
    return new_wd;
}

void add_basic_word(char* name, void(*func)(), uint8_t precedence) {
    uint32_t *new_wd = add_def(name, precedence);
    *(sys.cp) = (uint32_t) func;
    sys.cp++;
    sys.gloss_head = new_wd;
}

//The terminal/file text interpreter
void interpret() {
    while(*sys.idx != '\0') {
        while (isspace(*sys.idx)) sys.idx++;

        tick();
        if(*stack->at(0))
            execute();
        else

    }
}

int32_t main() {
    // Set up the FORTH system
    sys.sys = (uint32_t*) malloc(SYSTEM_SIZE * sizeof(uint32_t));
    sys.stack = sys.sys + (SYSTEM_SIZE / 2);
    sys.stack_0 = sys.stack;
    sys.rstack = sys.sys + 3*(SYSTEM_SIZE / 4);
    sys.rstack_0 = sys.rstack;
    sys.cp = sys.sys + (SYSTEM_SIZE / 4);
    *sys.cp = 0;
    sys.gloss_head = sys.cp;
    sys.cp++;
	sys.tib = (char*) sys.stack_0 + 1;
    sys.base = 10;

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
    add_basic_word("PAGE", page, 0);
    add_basic_word("QUIT", quit, 0);
    add_basic_word("?STACK", stack_q, 0);
    add_basic_word("SP@", sp_at, 0);
    add_basic_word("TIB", tib, 0);
    add_basic_word("#TIB", pound_tib, 0);
    add_basic_word("EXIT", exit_, 0);
    add_basic_word("<#", bracket_pound, 0);
    add_basic_word("#>", pound_bracket, 0);
    add_basic_word("#", pound, 0);
    add_basic_word("#S", pounds, 0);
    add_basic_word("HOLD", hold, 0);
    add_basic_word("SIGN", sign, 0);

    //Loop until terminal exit command is issued
    while(!sys.BYE) {
        printf("#F> ");
		fgets(sys.tib, 1024, stdin);
		sys.idx = sys.tib;
		sys.tib_len = strlen(sys.tib);
		for (int32_t i = 0; sys.idx[i]; i++)
			sys.idx[i] = toupper(sys.idx[i]);
        interpret();
        printf(" ok\n\n");
    }

    free(sys.sys);
    return 0;
}
