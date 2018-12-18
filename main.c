// C imports
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

// .h file imports
#include "stack.h"
#include "sys.h"
#include "defs.h"
#include "forth/intmath.h"
#include "forth/input.h"
#include "forth/logic.h"
#include "forth/output.h"
#include "forth/memory.h"
#include "forth/strmanip.h"
#include "forth/stackmanip.h"
#include "forth/rstack.h"
#include "forth/control.h"
#include "forth/misc.h"

struct system_t sys;
struct sys_util_t sys_util;

void (** func_table)() = NULL;
uint32_t ft_size = 0, ft_alloc = 0;

void add_func(void (*func)()) {
    if (ft_size == ft_alloc) {
        ft_alloc <<= 1;
        func_table = realloc(func_table, ft_alloc * sizeof(void (*)()));
    }

    func_table[ft_size] = func;
    ft_size++;
}


//Adds a new empty definition to the dictionary with the
//provided name and precedence, and returns a pointer to
//the end of the new definition
uint32_t *add_def(char *name, uint8_t precedence) {
    //Make sure the name gets stored as uppercase
    for(int i = 0; i < strlen(name); i++) {
        if(islower(name[i])) name[i] = toupper(name[i]);
    }
    //Set up traversal pointers
    uint32_t *new_wd = sys.cp;
    uint8_t *ccp = (uint8_t *) sys.cp;
    //Set precedence byte
    *ccp = precedence;
    ccp++;
    //Set string length byte
    uint8_t len = (uint8_t) strlen(name);
    uint8_t mem_len = ((len + 2) % 4 == 0) ? len : (len + 4 - ((len + 2) % 4));
    *ccp = mem_len;
    ccp++;
    //Copy the name into glossary memory
    memset(ccp, 0, mem_len);
    memcpy(ccp, name, len);
    ccp += mem_len;
    //Move & align system cp pointer
    sys.cp = (uint32_t*) ccp;
    //Set up back pointer
    *sys.cp = (uint32_t) sys.gloss_head;
    sys.cp++;
    return new_wd;
}

void add_basic_word(char* name, void(*func)(), uint8_t precedence) {
    uint32_t *new_wd = add_def(name, precedence);
    add_func(func);
    *(sys.cp) = ft_size - 1;
    sys.cp++;
    *(sys.cp) = EXIT_ADDR;
    sys.cp++;
    sys.gloss_head = new_wd;
    sys.old_cp = sys.cp;
}

int main() {
    // Set up the FORTH system
    ft_alloc = 32;
    func_table = malloc(sizeof(void (**)()) * ft_alloc);

    sys.sys = (uint32_t*) malloc(SYSTEM_SIZE * sizeof(uint32_t));
    sys.sys_base = 100000;
    sys.sys_top = sys.sys + SYSTEM_SIZE;
    sys.stack = sys.sys + (SYSTEM_SIZE / 2);
    sys.stack_0 = sys.stack;
    sys.rstack = sys.sys + 3*(SYSTEM_SIZE / 4);
    sys.rstack_0 = sys.rstack;
    sys.cp = sys.sys + (SYSTEM_SIZE / 4);
    *sys.cp = 0;
    sys.gloss_head = sys.cp;
    sys.gloss_base = sys.gloss_head;
    sys.cp++;
    sys.old_cp = sys.cp;
	sys.tib = (char*) sys.stack_0 + 1;
    sys.tib[0] = '\0';
    sys.idx = sys.tib;
    sys.idx_loc = 0;
    sys.tib_len = 4096;
    sys.base = 10;
    sys.inst = 0;
    sys.OKAY = false;
    sys.source_id = 0;

    //Build the glossary
    
    // Need to add EXIT first since add_basic_word depends on the table index of EXIT being 0
    add_basic_word("EXIT", exit_, 0);

    // Now need to add a few funcs that aren't words but need definite addresses in the table
    add_func(dnum_runtime);
    add_func(num_runtime);
    add_func(cond_jump);
    add_func(jump);
    add_func(do_runtime);
    add_func(loop_runtime);
    add_func(plus_loop_runtime);
    add_func(abort_quote_runtime);
    add_func(does_runtime);
    add_func(unloop);
   
    add_basic_word("!", store, 0);
    add_basic_word("#", pound, 0);
    add_basic_word("#>", pound_bracket, 0);
    add_basic_word("#S", pounds, 0);
    add_basic_word("'", tick, 0);
    add_basic_word("(", paren, 1);
    add_basic_word("*", mult, 0);
    add_basic_word("*/", multDiv, 0);
    add_basic_word("*/MOD", multDivMod, 0);
    add_basic_word("+", add, 0);
    add_basic_word("+!", plus_store, 0); 
    add_basic_word("+LOOP", plus_loop, 1);
    add_basic_word(",", comma, 0);
    add_basic_word("-", sub, 0);
    add_basic_word(".", print, 0);
    add_basic_word(".\"", dot_quote, 1);
    add_basic_word("/", divd, 0);
    add_basic_word("/MOD", modDiv, 0);
    add_basic_word("0<", zeroLessThan, 0);
    add_basic_word("0=", zeroEquals, 0);
    add_basic_word("1+", add1, 0);
    add_basic_word("1-", sub1, 0);
    add_basic_word("2!", store2, 0);
    add_basic_word("2*", lshift, 0);
    add_basic_word("2/", rshift, 0);
    add_basic_word("2@", fetch2, 0);
    add_basic_word("2DROP", drop2, 0);
    add_basic_word("2DUP", dup2, 0);
    add_basic_word("2OVER", over2, 0);
    add_basic_word("2SWAP", swap2, 0);
    add_basic_word(":", colon, 0);
    add_basic_word(";", semicolon, 1);
    add_basic_word("<", lessThan, 0);
    add_basic_word("<#", bracket_pound, 0);
    add_basic_word("=", equals, 0);
    add_basic_word(">", greaterThan, 0);
    add_basic_word(">BODY", to_body, 0);
    add_basic_word(">IN", to_in, 0);
    add_basic_word(">NUMBER", to_number, 0);
    add_basic_word(">R", retPush, 0);
    add_basic_word("?DUP", dup_if, 0);
    add_basic_word("@", fetch, 0);
    add_basic_word("ABORT", abort_, 0);
    add_basic_word("ABORT\"", abort_quote, 1);
    add_basic_word("ABS", abs_, 0);
    add_basic_word("ACCEPT", accept, 0);
    add_basic_word("ALIGN", align, 0);
    add_basic_word("ALIGNED", aligned, 0);
    add_basic_word("ALLOT", allot, 0);
    add_basic_word("AND", and_, 0);
    add_basic_word("BASE", base, 0);
    add_basic_word("BEGIN", begin, 1);
    add_basic_word("BL", bl, 0);
    add_basic_word("BYE", bye, 0);
    add_basic_word("C!", c_store, 0); 
    add_basic_word("C,", c_comma, 0);
    add_basic_word("C@", c_fetch, 0); 
    add_basic_word("CELL+", cell_plus, 0); 
    add_basic_word("CELLS", cells, 0);
    add_basic_word("CHAR", char_, 0);
    add_basic_word("CHAR+", char_plus, 0);
    add_basic_word("CHARS", chars, 0);
    add_basic_word("CONSTANT", constant, 0);
    add_basic_word("COUNT", count, 0);
    add_basic_word("CR", cr, 0);
    add_basic_word("CREATE", create, 0);
    add_basic_word("DECIMAL", decimal, 0);
    add_basic_word("DEPTH", depth, 0);
    add_basic_word("DO", do_, 1);
    add_basic_word("DOES>", does, 0);
    add_basic_word("DROP", drop, 0);
    add_basic_word("DUP", dup, 0);
    add_basic_word("ELSE", else_, 1);
    add_basic_word("EMIT", emit, 0);
    add_basic_word("ENVIRONMENT?", environment, 0);
    add_basic_word("EVALUATE", evaluate, 0); 
    add_basic_word("EXECUTE", execute, 0);
    add_basic_word("FILL", fill, 0);
    add_basic_word("FIND", find, 0);
    add_basic_word("FM/MOD", FmodDiv, 0); 
    add_basic_word("HERE", here, 0);
    add_basic_word("HOLD", hold, 0);
    add_basic_word("I", retCopy, 0);
    add_basic_word("IF", if_, 1);
    add_basic_word("IMMEDIATE", immediate, 0);
    add_basic_word("INVERT", invert, 0);
    add_basic_word("J", retCopy3, 0);
    add_basic_word("KEY", key, 0);
    add_basic_word("LEAVE", leave, 1);
    add_basic_word("LITERAL", literal, 1);
    add_basic_word("LOOP", loop, 1);
    add_basic_word("LSHIFT", lshift, 0);
    add_basic_word("M*", Mmult, 0);
    add_basic_word("MAX", max, 0);
    add_basic_word("MIN", min, 0);
    add_basic_word("MOD", mod, 0);
    add_basic_word("MOVE", move, 0);
    add_basic_word("NEGATE", neg, 0);
    add_basic_word("OR", or_, 0);
    add_basic_word("OVER", over, 0);
    add_basic_word("POSTPONE", postpone, 1);
    add_basic_word("QUIT", quit, 0);
    add_basic_word("R>", retPop, 0);
    add_basic_word("R@", retCopy, 0); 
    add_basic_word("RECURSE", recurse, 1);
    add_basic_word("REPEAT", repeat, 1);
    add_basic_word("ROT", rot, 0);
    add_basic_word("RSHIFT", rshift, 0);
    add_basic_word("S\"", s_quote, 1);
    add_basic_word("S>D", stod, 0);
    add_basic_word("SIGN", sign, 0);
    add_basic_word("SM/REM", SmodDiv, 0);
    add_basic_word("SOURCE", source, 0);
    add_basic_word("SPACE", space, 0);
    add_basic_word("SPACES", spaces, 0); 
    add_basic_word("STATE", state, 0);
    add_basic_word("SWAP", swap, 0);
    add_basic_word("THEN", then, 1);
    add_basic_word("TYPE", type, 0);
    add_basic_word("U.", uprint, 0);
    add_basic_word("U<", UlessThan, 0);
    add_basic_word("UM*", umult, 0);
    add_basic_word("UM/MOD", UmodDiv, 0);
    add_basic_word("UNLOOP", unloop, 0);
    add_basic_word("UNTIL", until, 1);
    add_basic_word("VARIABLE", variable, 0);
    add_basic_word("WHILE", while_, 1);
    add_basic_word("WORD", word, 0);
    add_basic_word("XOR", xor, 0);
    add_basic_word("[", rbracket, 1);
    add_basic_word("[']", bracket_tick_bracket, 1);
    add_basic_word("[CHAR]", bracket_char_bracket, 1);
    add_basic_word("]", lbracket, 1);
    

    unsigned char name[5] = "\x04QUIT";
    stack_push((uint32_t) name);
    find();
    stack_pop(1);
    sys.q_addr = (uint32_t*)*stack_at(0);
    stack_pop(1);
    exec(sys.q_addr);

    free(sys.sys);
    return 0;
}
