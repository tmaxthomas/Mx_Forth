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

//Structural (branching) words
void cond();
void loop();
void loop_plus();
void do_();
void nop();
void leave();

//Reflective words
void tick();
void execute();

//Misc. words
void page();
void quit();
void exit();
void abort_();
void stack_q();

/*
//Finds words in the glossary
Function* find(std::string& name) {
    for(auto itr = glossary.rbegin(); itr != glossary.rend(); itr++)
        if(itr->first == name)
            return itr->second;
    return NULL;
}


//Finds compile-time and run-time words in the glossaries
Function* comp_find(std::string& name) {
    Function* func = find(name);
    if(func)
        return func;
    for(auto itr = comp_glossary.rbegin(); itr != comp_glossary.rend(); itr++)
        if(itr->first == name)
            return itr->second;
    return NULL;
}

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


//Number & related factory method
Function* make_num(std::string& str) {
    if(!is_num(str)) {
        printf("%s ?", str.c_str());
        abort_();
    }

    bool db = false;

    if(str[str.size() - 1] == '.') {
        db = true;
        str.erase(str.size() - 1, 1);
    }

    if(db)
        return new DoubleConst(atol(str.c_str();
    else
        return new Var(atoi(str.c_str()), 4);
}

//Adds user-defined words to the glossary

//WARNING: This code is very obtuse, despite the extensive comments. This program is necessarily
//full of crazy pointer math, and this is no exception.
void add_word() {
    std::stack<Function*> if_stack, do_stack, begin_stack, while_stack;
    std::stack<std::vector<Function*> > leave_stack;
    char *tmp_buf;
    size_t i;
    char *tmp_idx;
    GetSubstring(isspace(*tmp_idx));
    std::string name(tmp_buf), func = "";
    free(tmp_buf);
    //Declare a starting null node
    Function *head = nop), *tail = head;
    GetSubstring(isspace(*tmp_idx));
    func = std::string(tmp_buf);

    while(func != ";") {
        //Comment handler
        if(func == "(") {
            GetSubstring(*tmp_idx == ')');
        } else if(func == ".\"") {                       //Handles string printing
            GetSubstring(*tmp_idx == '"');
            StrPrint *node = new StrPrint(tmp_buf);
            FuncAlloc(tail, 1);
            tail->next[0] = node;
            tail = node;
        } else if(func == "ABORT\"") {                   //Handles ABORT"
            GetSubstring(*tmp_idx == '"');
            Abort *node = new Abort(tmp_buf);
            FuncAlloc(tail, 1);
            tail->next[0] = node;
            tail = node;
        } else if(func == "IF") {                        //Conditional handling, step 1
            Function* if_ = cond);          //Allcoate & initialize the if node
            FuncAlloc(tail, 1);
            tail->next[0] = if_;                         //Tail points to if
            FuncAlloc(tail->next[0], 2);                 //if block branching nodes declaration
            tail->next[0]->next[1] = nop);  //Dummy node for conditional branch in order to have if head node location
            if_stack.push(tail->next[0]);                //Push the node onto the conditional stack
            tail = if_stack.top()->next[1];              //Set tail node
        } else if(func == "ELSE") {
            Function* else_tail;
            else_tail = if_stack.top();                  //Grab the top of the conditional stack
            if_stack.top() = tail;                       //Move the tail
            else_tail->next[0] = nop);      //Allocate dummy node
            tail = else_tail->next[0];
            Function* then = nop);          //Another dummy node, to unite the two conditional paths
            FuncAlloc(tail, 1);                          //Stitch the active tail into the dummy node
            tail->next[0] = then;
            if(!if_stack.top()->next)                    //Handle possible if/else
                FuncAlloc(if_stack.top(), 1);
            if_stack.top()->next[0] = then;              //Stitch the other tail into the dummy node
            tail = tail->next[0];
            if_stack.pop();                              //Clean up the conditional stack
        } else if(func == "THEN") {
            Function* then = nop);          //Strucutral merger node allocation
            FuncAlloc(tail, 1);
            tail->next[0] = then;                        //Tie the tail into the node
            if(!if_stack.top()->next)                    //Make sure the if-branch can be tied into the node
                FuncAlloc(if_stack.top(), 1);
            if_stack.top()->next[0] = then;              //Tie the if_stack tail into the node
            tail = tail->next[0];
            if_stack.pop();
        } else if(func == "DO") {
            Function *_do = do_);           //Allocate do function
            FuncAlloc(tail, 1);
            tail->next[0] = _do;                         //Set tail->next
            tail = tail->next[0];                        //Move tail
            Function *loop_head = nop);     //Allocate loop block head
            FuncAlloc(tail, 1);
            tail->next[0] = loop_head;                   //Move tail to loop head
            do_stack.push(loop_head);                    //Put head on do loop stack
            tail = tail->next[0];
            std::vector<Function *> temp;
            leave_stack.push(temp);                      //Set up leave stack
        } else if(func == "LEAVE") {
            Function *leave_ = leave);      //Allocate leave node
            FuncAlloc(tail, 1);
            tail->next[0] = leave_;                      //Set tail->next
            leave_stack.top().push_back(leave_);         //Push node onto leave stack
            tail = tail->next[0];
            FuncAlloc(tail, 2);                          //Set up leave escape path
            tail->next[0] = nop);
            tail = tail->next[0];
        } else if(func == "LOOP" || func == "+LOOP") {
            Function *loop_;
            if (func == "LOOP")
                loop_ = loop);              //Allocate loop escape checker function
            else
                loop_ = loop_plus);         //Differentiate between LOOP and +LOOP
            FuncAlloc(tail, 2);
            tail->next[0] = loop_;                       //Add loop escape checker to loop path
            FuncAlloc(loop_, 2);                         //Allocate loop branching
            loop_->next[1] = do_stack.top();             //Plug loop path into loop head
            do_stack.pop();                              //Clean up do stack
            loop_->next[0] = nop);          //Set up loop escape path
            tail = loop_->next[0];                       //Move tail
            while (!leave_stack.top().empty()) {         //Take care of any leave's
                leave_stack.top().back()->next[1] = tail;
                leave_stack.top().pop_back();
            }
            leave_stack.pop();
        } else if (func == "BEGIN") {
            Function *begin = nop);         //Allocate definite loop head
            FuncAlloc(tail, 1);
            tail->next[0] = begin;                       //Set tail->next
            begin_stack.push(begin);                     //Push loop head onto stack
            tail = tail->next[0];                        //Move tail
        } else if (func == "UNTIL") {
            Function* until = cond);        //Allocate until conditional
            FuncAlloc(tail, 1);
            tail->next[0] = until;                       //Set tail->next
            tail = tail->next[0];                        //Move tail
            FuncAlloc(tail, 2);
            tail->next[0] = begin_stack.top();           //Point32_t the conditional false path at loop head
            begin_stack.pop();                           //Clean up loop stack
            Function* temp = nop);          //Set up escape tail
            tail->next[1] = temp;
            tail = tail->next[1];
        } else if (func == "WHILE") {
            Function* while_ = cond);       //Allocate while conditional
            FuncAlloc(tail, 1);
            tail->next[0] = while_;                      //Set tail->next
            tail = tail->next[0];                        //Move tail->next
            FuncAlloc(tail, 2);                          //Allocate branching
            Function* temp = nop);
            tail->next[1] = temp;                        //Set up true path
            temp = nop);
            tail->next[0] = temp;                        //Set up false path
            while_stack.push(temp);                      //Push false path head onto while stack
            tail = tail->next[1];                        //Move tail
        } else if (func == "REPEAT") {
            FuncAlloc(tail, 1);
            tail->next[0] = begin_stack.top();           //Plug current tail into loop head
            begin_stack.pop();                           //Clean up begin_stack
            tail = while_stack.top();                    //Move current tail to loop escape path
            while_stack.pop();                           //Clean up while_stack
        } else if (func == name) {                       //Allow for recursive calls
            FuncAlloc(tail, 1);
            tail->next[0] = (Function*) new UsrFunc(head);
            tail = tail->next[0];
        } else if (func == "[']") {
            GetSubstring(isspace(*tmp_idx));
            std::string str(tmp_buf);
            Function* ptr = find(str);
            FuncAlloc(tail, 1);
            tail->next[0] = (Function*) new Var((int32_t) ptr, 4);
            tail = tail->next[0];
        } else if (func == "[CHAR]") {                   //Compile a character literal into the definition
            GetSubstring(isspace(*tmp_idx));
            std::string str(tmp_buf);
            if(str.size() != 1) {
                printf("%s ?", str.c_str());
                abort_();
            }
            FuncAlloc(tail, 1);
            tail->next[0] = new Var(str[0], 4);
            tail = tail->next[0];
        } else {
            Function *temp;
            Function *tmp_ptr = comp_find(func);
            //Figure out what the heck kind of thing we're dealing with
            temp = (Function *) (tmp_ptr ? (tmp_ptr->next ? new UsrFunc(tmp_ptr) : tmp_ptr)) : make_num(func));
            FuncAlloc(tail, 1);
            tail->next[0] = temp;
            tail = tail->next[0];
        }
        while (isspace(*sys.idx)) sys.idx++;                        //Take care of loose/excess whitespace
        GetSubstring(isspace(*tmp_idx));
        func = std::string(tmp_buf);
    }
    add_basic_word(name, head));
}
*/
//Word-execution wrapper - executed word pointed to by func
//Written to help debugging and to avoid stack overflow resulting from excessive recursion
/*
void run(Function* func) {
    while(func->next) {
        int32_t idx = func->run();
        if(idx == -1) return;
        else func = func->next[idx];
    }
    func->run();
}
*/

// ( -- addr )
// Pushes the exectuion address of the next word in the input stream onto the stack
void tick() {
    char *tmp_buf, *tmp_idx;
    size_t i;
    GetSubstring(isspace(*tmp_idx));
}

// ( addr -- )
// Executes the word pointed to by addr
void execute() {
    stack_pop(1);
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

// --------------------------------------------------------------
// STRUCTURAL/COMPILE-ONLY WORDS START HERE
// --------------------------------------------------------------

/*
//Manages branching for if statements
//Branches to 0 if false, or to 1 if true.
int32_t cond() {
    int32_t a = *stack_at(0) != 0;
    stack_pop(1);
    return a;
}

//Initializes definite loops
int32_t do_() {
    int32_t index = *stack_at(0);
    int32_t limit = *stack_at(1);
    stack_pop(2);
    rstack_push(limit);
    rstack_push(index);
    return 0;
}

//Definite loop conditional
int32_t loop() {
    int32_t index = *(int32_t*)rstack_at(0);
    int32_t limit = *(int32_t*)rstack_at(1);
    index++;
    if(index != limit) {
        *(int32_t*)rstack_at(0) = index;
        return 1;
    } else {
        rstack_pop(2);
        return 0;
    }
}

//Why does FORTH have to be inconsistent with its loop end condition, anyways?
int32_t loop_plus() {
    int32_t index = *(int32_t*)rstack_at(0);
    int32_t limit = *(int32_t*)rstack_at(1);
    int32_t inc = *(int32_t*)stack_at(0);
    index += inc;
    if(inc < 0) {
        if(index >= limit) {
            *(int32_t*)rstack_at(0) = index;
            return 1;
        } else {
            rstack_pop(2);
            return 0;
        }
    } else {
        if(index < limit) {
            *(int32_t*)rstack_at(0) = index;
            return 1;
        } else {
            rstack_pop(2);
            return 0;
        }
    }
}

//Null operand for structural nodes
int32_t nop() {
    return 0; //That's right; it does nothing.
}

//Nop-esque operand to handle loop break pathing
int32_t leave() {
    return 1;
}

//Pushes a number onto the stack
void number(std::string& str) {
    Function* f = make_num(str);
    f->run();
}
*/

void run_basic_word() {
    void(**func)() = (void(**)()) sys.inst;
    func++;
    (*func)();
}

uint32_t* add_def(char* name, uint8_t precedence) {
    uint32_t *new_wd = sys.cp;
    uint8_t *ccp = (uint8_t*) sys.cp;
    *ccp = precedence;
    ccp++;
    uint8_t len = (uint8_t) strlen(name);
    *ccp = len;
    ccp++;
    memcpy(ccp, name, len);
    ccp += len;
    uint32_t ccp_val = (uint32_t) ccp;
    //Maintain alignment
    ccp_val += ccp_val % 4;
    sys.cp = (uint32_t*) ccp_val;
    sys.cp++;
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
void text_interpreter() {
    while(*sys.idx != '\0') {
        while (isspace(*sys.idx)) sys.idx++;
        // Get the next word in the input stream
        char *tmp_buf;
        size_t i;
        char *tmp_idx;
        GetSubstring(isspace(*tmp_idx));

        free(tmp_buf);
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
    add_basic_word("*//*", multDiv, 0);
    add_basic_word("*//*MOD", multDivMod, 0);
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

        printf(" ok\n\n");
    }

    free(sys.sys);
    return 0;
}
