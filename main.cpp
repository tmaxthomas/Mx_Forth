// C imports
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>

// C++ imports
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <unordered_set>

// .h file imports
#include "stack.h"
#include "Function.h"
#include "sys.h"
#include "forth/intmath.h"
#include "forth/logic.h"
#include "forth/output.h"
#include "forth/memory.h"
#include "forth/strmanip.h"
#include "forth/stackmanip.h"
#include "forth/rstack.h"

struct System sys;

//Utility macro for getting char-delimited substrings of C strings
#define GetSubstring(boolean) tmp_idx = sys.idx; for (i = 0; !(boolean); i++) tmp_idx++; \
                           tmp_buf = (char *) malloc(i + 1); for (size_t j = 0; j < i; j++) tmp_buf[j] = sys.idx[j]; \
                           tmp_buf[i] = 0; sys.idx = tmp_idx; if (*sys.idx) sys.idx++

//Utility macro for Function next array allocation
#define FuncAlloc(func, num) func->next = new Function *[num]; func->size = num

//Utility macro for variable creation
#define MakeVar(type) GetSubstring(isspace(*tmp_idx)); Function *f = new Function(nop); FuncAlloc(f, 1); \
                      f->next[0] = new type; glossary.push_back(std::make_pair(tmp_buf, f))

//Global boolean flags for program state management
bool ABORT = false, BYE = false, QUIT = false, S_UND = false, PAGE = false;

//Stack *stack, *return_stack;

std::vector<std::pair<std::string, Function*> > glossary, comp_glossary;

//Structural (branching) words
int32_t cond();
int32_t loop();
int32_t loop_plus();
int32_t do_();
int32_t nop();
int32_t leave();

//Reflective words
int32_t tick();
int32_t execute();

//Misc. words
int32_t page();
int32_t quit();
int32_t exit();
int32_t abort_();
int32_t stack_q();

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
        return new DoubleConst(atol(str.c_str()));
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
    Function *head = new Function(nop), *tail = head;
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
            Function* if_ = new Function(cond);          //Allcoate & initialize the if node
            FuncAlloc(tail, 1);
            tail->next[0] = if_;                         //Tail points to if
            FuncAlloc(tail->next[0], 2);                 //if block branching nodes declaration
            tail->next[0]->next[1] = new Function(nop);  //Dummy node for conditional branch in order to have if head node location
            if_stack.push(tail->next[0]);                //Push the node onto the conditional stack
            tail = if_stack.top()->next[1];              //Set tail node
        } else if(func == "ELSE") {
            Function* else_tail;
            else_tail = if_stack.top();                  //Grab the top of the conditional stack
            if_stack.top() = tail;                       //Move the tail
            else_tail->next[0] = new Function(nop);      //Allocate dummy node
            tail = else_tail->next[0];
            Function* then = new Function(nop);          //Another dummy node, to unite the two conditional paths
            FuncAlloc(tail, 1);                          //Stitch the active tail into the dummy node
            tail->next[0] = then;
            if(!if_stack.top()->next)                    //Handle possible if/else
                FuncAlloc(if_stack.top(), 1);
            if_stack.top()->next[0] = then;              //Stitch the other tail into the dummy node
            tail = tail->next[0];
            if_stack.pop();                              //Clean up the conditional stack
        } else if(func == "THEN") {
            Function* then = new Function(nop);          //Strucutral merger node allocation
            FuncAlloc(tail, 1);
            tail->next[0] = then;                        //Tie the tail into the node
            if(!if_stack.top()->next)                    //Make sure the if-branch can be tied into the node
                FuncAlloc(if_stack.top(), 1);
            if_stack.top()->next[0] = then;              //Tie the if_stack tail into the node
            tail = tail->next[0];
            if_stack.pop();
        } else if(func == "DO") {
            Function *_do = new Function(do_);           //Allocate do function
            FuncAlloc(tail, 1);
            tail->next[0] = _do;                         //Set tail->next
            tail = tail->next[0];                        //Move tail
            Function *loop_head = new Function(nop);     //Allocate loop block head
            FuncAlloc(tail, 1);
            tail->next[0] = loop_head;                   //Move tail to loop head
            do_stack.push(loop_head);                    //Put head on do loop stack
            tail = tail->next[0];
            std::vector<Function *> temp;
            leave_stack.push(temp);                      //Set up leave stack
        } else if(func == "LEAVE") {
            Function *leave_ = new Function(leave);      //Allocate leave node
            FuncAlloc(tail, 1);
            tail->next[0] = leave_;                      //Set tail->next
            leave_stack.top().push_back(leave_);         //Push node onto leave stack
            tail = tail->next[0];
            FuncAlloc(tail, 2);                          //Set up leave escape path
            tail->next[0] = new Function(nop);
            tail = tail->next[0];
        } else if(func == "LOOP" || func == "+LOOP") {
            Function *loop_;
            if (func == "LOOP")
                loop_ = new Function(loop);              //Allocate loop escape checker function
            else
                loop_ = new Function(loop_plus);         //Differentiate between LOOP and +LOOP
            FuncAlloc(tail, 2);
            tail->next[0] = loop_;                       //Add loop escape checker to loop path
            FuncAlloc(loop_, 2);                         //Allocate loop branching
            loop_->next[1] = do_stack.top();             //Plug loop path into loop head
            do_stack.pop();                              //Clean up do stack
            loop_->next[0] = new Function(nop);          //Set up loop escape path
            tail = loop_->next[0];                       //Move tail
            while (!leave_stack.top().empty()) {         //Take care of any leave's
                leave_stack.top().back()->next[1] = tail;
                leave_stack.top().pop_back();
            }
            leave_stack.pop();
        } else if (func == "BEGIN") {
            Function *begin = new Function(nop);         //Allocate definite loop head
            FuncAlloc(tail, 1);
            tail->next[0] = begin;                       //Set tail->next
            begin_stack.push(begin);                     //Push loop head onto stack
            tail = tail->next[0];                        //Move tail
        } else if (func == "UNTIL") {
            Function* until = new Function(cond);        //Allocate until conditional
            FuncAlloc(tail, 1);
            tail->next[0] = until;                       //Set tail->next
            tail = tail->next[0];                        //Move tail
            FuncAlloc(tail, 2);
            tail->next[0] = begin_stack.top();           //Point32_t the conditional false path at loop head
            begin_stack.pop();                           //Clean up loop stack
            Function* temp = new Function(nop);          //Set up escape tail
            tail->next[1] = temp;
            tail = tail->next[1];
        } else if (func == "WHILE") {
            Function* while_ = new Function(cond);       //Allocate while conditional
            FuncAlloc(tail, 1);
            tail->next[0] = while_;                      //Set tail->next
            tail = tail->next[0];                        //Move tail->next
            FuncAlloc(tail, 2);                          //Allocate branching
            Function* temp = new Function(nop);
            tail->next[1] = temp;                        //Set up true path
            temp = new Function(nop);
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
            temp = (Function *) (tmp_ptr ? (tmp_ptr->next ? new UsrFunc(tmp_ptr) : new Function(tmp_ptr)) : make_num(func));
            FuncAlloc(tail, 1);
            tail->next[0] = temp;
            tail = tail->next[0];
        }
        while (isspace(*sys.idx)) sys.idx++;                        //Take care of loose/excess whitespace
        GetSubstring(isspace(*tmp_idx));
        func = std::string(tmp_buf);
    }
    glossary.push_back(std::make_pair(name, head));
}

//Word-execution wrapper - executed word pointed to by func
//Written to help debugging and to avoid stack overflow resulting from excessive recursion
void run(Function* func) {
    while(func->next) {
        int32_t idx = func->run();
        if(idx == -1) return;
        else func = func->next[idx];
    }
    func->run();
}

// ( -- addr )
// Pushes the exectuion address of the next word in the input stream onto the stack
int32_t tick() {
    char *tmp_buf, *tmp_idx;
    size_t i;
    GetSubstring(isspace(*tmp_idx));
    std::string str(tmp_buf);
    Function* func = find(str);
    stack_push((int32_t) func);
    return 0;
}

// ( addr -- )
// Executes the word pointed to by addr
int32_t execute() {
    Function* func = (Function*)*stack_at(0);
    stack_pop(1);
    run(func);
    return 0;
}

// ( -- )
//Clears the screen
int32_t page() {
    system("clear");
    QUIT = true; //Avoid printing 'ok' after clearing the terminal, because it looks weird
    PAGE = true; //Avoid printing a double newline as well, because it too looks weird
    return 0;
}

// ( -- )
//Aborts the program
int32_t abort_() {
    ABORT = true;
    stack_clear();
    throw 1;
}

// ( -- )
//Sets up the interpreter to not print32_t ok, among other things
int32_t quit() {
    QUIT = true;
    rstack_clear();
    return -1;
}

// ( -- f )
// Pushes true if the stack is empty, pushes false otherwise
int32_t stack_q() {
    if(sys.stack == sys.stack_0) stack_push((int32_t)0xffffffff);
    else stack_push(0x00000000);
    return 0;
}

// ( -- )
// Compiled only-used to trigger termination of word execution
int32_t exit() {
    return -1;
}

// --------------------------------------------------------------
// STRUCTURAL/COMPILE-ONLY WORDS START HERE
// --------------------------------------------------------------

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

//The terminal/file text interpreter
//Parses words (like :, VARIABLE) that can't be put into definitions
void text_interpreter() {
    while(*sys.idx != '\0') {
        while (isspace(*sys.idx)) sys.idx++;
        // Get the next word in the input stream
        char *tmp_buf;
        size_t i;
        char *tmp_idx;
        GetSubstring(isspace(*tmp_idx));
        std::string str(tmp_buf);
        free(tmp_buf);

        //Once found, parse the word
        Function *func = find(str);

        // Creation words
        if (str == ":")
            add_word();
        else if (str == "VARIABLE") {
            MakeVar(Var((int32_t) malloc(4), 4));
        } else if(str == "2VARIABLE") {
            MakeVar(Var((int32_t) malloc(8), 8));
        } else if(str == "CONSTANT") {
            MakeVar(Var(*(int32_t *) stack_at(0), 4));
            stack_pop(1);
        } else if(str == "2CONSTANT") {
            MakeVar(DoubleConst(*(int64_t*)stack_at(0)));
            stack_pop(2);
        } else if(str == "CREATE") {
            GetSubstring(isspace(*tmp_idx));
            glossary.push_back(std::make_pair(tmp_buf, new Var(0, 0)));

        // Allotment words
        } else if(str == "ALLOT") {
            Var *v = dynamic_cast<Var *>(glossary.back().second);
            if(v) {
                uint32_t size = *stack_at(0);
                stack_pop(1);
                v->n = (int32_t) realloc((void *) v->n, v->s + size);
                v->s += size;
            }
        } else if(str == ",") {
            Var *v = dynamic_cast<Var *>(glossary.back().second);
            if (v) {
                int32_t n = *(int32_t *) stack_at(0);
                stack_pop(1);
                v->n = (int32_t) realloc((void *) v->n, v->s + 4);
                int32_t *pt = (int32_t *) (v->n + v->s);
                *pt = n;
                v->s += 4;
            }
        } else if(str == "C,") {
            Var *v = dynamic_cast<Var *>(glossary.back().second);
            if(v) {
                char c = *(char*) stack_at(0);
                stack_pop(1);
                v->n = (int32_t) realloc((void *) v->n, v->s + 1);
                char *pt = (char *) (v->n + v->s);
                *pt = c;
                v->s += 1;
            }

        // I/O words
        } else if (str == "INCLUDE") {
            char r = 'r';
            GetSubstring(isspace(*tmp_idx));
            FILE* curr_file;

            curr_file = fopen(tmp_buf, &r);
            fseek(curr_file, 0, SEEK_END);
            size_t n = (size_t) ftell(curr_file);
            rewind(curr_file);

			//Save *idx and *buf
			char *buf_ = sys.buf, *idx_ = sys.idx;
			//Read some stuff into a new buf
            char* buf = (char*) malloc(n + 1);
            fread(buf, n, 1, curr_file);
            buf[n] = '\0';
            sys.idx = buf;
            text_interpreter();
            free(buf);
            sys.idx = idx_;
			sys.buf = buf_;
            free(tmp_buf);
            fclose(curr_file);
        } else if (str == ".\"") {
                GetSubstring(*tmp_idx == '"');
                printf("%s", tmp_buf);
                free(tmp_buf);

        // Misc. words
        } else if (str == "FORGET")
            forget();
        else if (str == "BYE")
            BYE = true;
        else if (func)
            run(func);
        else
            number(str);
    }
}

int32_t main() {
    // Set up FORTH system and do all of the pointer math
    sys.sys = (uint32_t*) malloc(262144);
    sys.stack = sys.sys + 32768;
    sys.stack_0 = sys.stack;
    sys.rstack = sys.sys + 49152;
    sys.rstack_0 = sys.rstack;
	sys.buf = (char*) sys.stack_0 + 1;

    //Build the glossary
    glossary.push_back(std::make_pair("CR", new Function(cr)));
    glossary.push_back(std::make_pair("SPACES", new Function(spaces)));
    glossary.push_back(std::make_pair("SPACE", new Function(space)));
    glossary.push_back(std::make_pair("EMIT", new Function(emit)));
    glossary.push_back(std::make_pair(".", new Function(print)));
    glossary.push_back(std::make_pair("U.", new Function(uprint)));
    glossary.push_back(std::make_pair("D.", new Function(dprint)));
    glossary.push_back(std::make_pair("U.R", new Function(urjprint)));
    glossary.push_back(std::make_pair("D.R", new Function(drjprint)));
    glossary.push_back(std::make_pair(".S", new Function(printS)));
    glossary.push_back(std::make_pair("TYPE", new Function(type)));
    glossary.push_back(std::make_pair("-TRAILING", new Function(trailing)));
    glossary.push_back(std::make_pair("+", new Function(add)));
    glossary.push_back(std::make_pair("D+", new Function(Dadd)));
    glossary.push_back(std::make_pair("M+", new Function(Madd)));
    glossary.push_back(std::make_pair("-", new Function(sub)));
    glossary.push_back(std::make_pair("D-", new Function(Dsub)));
    glossary.push_back(std::make_pair("*", new Function(mult)));
    glossary.push_back(std::make_pair("UM*", new Function(umult)));
    glossary.push_back(std::make_pair("M*", new Function(Mmult)));
    glossary.push_back(std::make_pair("/", new Function(div)));
    glossary.push_back(std::make_pair("MOD", new Function(mod)));
    glossary.push_back(std::make_pair("/MOD", new Function(modDiv)));
    glossary.push_back(std::make_pair("UM/MOD", new Function(UmodDiv)));
    glossary.push_back(std::make_pair("SM/REM", new Function(SmodDiv)));
    glossary.push_back(std::make_pair("FM/MOD", new Function(FmodDiv)));
    glossary.push_back(std::make_pair("*/", new Function(multDiv)));
    glossary.push_back(std::make_pair("*/MOD", new Function(multDivMod)));
    glossary.push_back(std::make_pair("1+", new Function(add1)));
    glossary.push_back(std::make_pair("1-", new Function(sub1)));
    glossary.push_back(std::make_pair("2+", new Function(add2)));
    glossary.push_back(std::make_pair("2-", new Function(sub2)));
    glossary.push_back(std::make_pair("2*", new Function(lshift)));
    glossary.push_back(std::make_pair("2/", new Function(rshift)));
    glossary.push_back(std::make_pair("AND", new Function(and_)));
    glossary.push_back(std::make_pair("OR", new Function(or_)));
    glossary.push_back(std::make_pair("ABS", new Function(abs)));
    glossary.push_back(std::make_pair("DABS", new Function(dabs)));
    glossary.push_back(std::make_pair("NEGATE", new Function(neg)));
    glossary.push_back(std::make_pair("DNEGATE", new Function(Dneg)));
    glossary.push_back(std::make_pair("MIN", new Function(min)));
    glossary.push_back(std::make_pair("DMIN", new Function(Dmin)));
    glossary.push_back(std::make_pair("MAX", new Function(max)));
    glossary.push_back(std::make_pair("DMAX", new Function(Dmax)));
    glossary.push_back(std::make_pair("SWAP", new Function(swap)));
    glossary.push_back(std::make_pair("2SWAP", new Function(swap2)));
    glossary.push_back(std::make_pair("DUP", new Function(dup)));
    glossary.push_back(std::make_pair("2DUP", new Function(dup2)));
    glossary.push_back(std::make_pair("?DUP", new Function(dup_if)));
    glossary.push_back(std::make_pair("OVER", new Function(over)));
    glossary.push_back(std::make_pair("2OVER", new Function(over2)));
    glossary.push_back(std::make_pair("ROT", new Function(rot)));
    glossary.push_back(std::make_pair("DROP", new Function(drop)));
    glossary.push_back(std::make_pair("DROP2", new Function(drop2)));
    glossary.push_back(std::make_pair("TUCK", new Function(tuck)));
    glossary.push_back(std::make_pair(">R", new Function(retPush)));
    glossary.push_back(std::make_pair("R>", new Function(retPop)));
    glossary.push_back(std::make_pair("I", new Function(retCopy)));
    glossary.push_back(std::make_pair("R@", new Function(retCopy)));
    glossary.push_back(std::make_pair("J", new Function(retCopy3)));
    glossary.push_back(std::make_pair("=", new Function(equals)));
    glossary.push_back(std::make_pair("D=", new Function(Dequals)));
    glossary.push_back(std::make_pair("<", new Function(lessThan)));
    glossary.push_back(std::make_pair("U<", new Function(UlessThan)));
    glossary.push_back(std::make_pair("D<", new Function(DlessThan)));
    glossary.push_back(std::make_pair("DU<", new Function(DUlessThan)));
    glossary.push_back(std::make_pair(">", new Function(greaterThan)));
    glossary.push_back(std::make_pair("0=", new Function(zeroEquals)));
    glossary.push_back(std::make_pair("D0=", new Function(DzeroEquals)));
    glossary.push_back(std::make_pair("0<", new Function(zeroLessThan)));
    glossary.push_back(std::make_pair("0>", new Function(zeroGreaterThan)));
    glossary.push_back(std::make_pair("!", new Function(store)));
    glossary.push_back(std::make_pair("2!", new Function(store2)));
    glossary.push_back(std::make_pair("C!", new Function(c_store)));
    glossary.push_back(std::make_pair("+!", new Function(plus_store)));
    glossary.push_back(std::make_pair("@", new Function(fetch)));
    glossary.push_back(std::make_pair("2@", new Function(fetch2)));
    glossary.push_back(std::make_pair("C@", new Function(c_fetch)));
    glossary.push_back(std::make_pair("?", new Function(query)));
    glossary.push_back(std::make_pair("CELL", new Function(cell)));
    glossary.push_back(std::make_pair("CELLS", new Function(cells)));
    glossary.push_back(std::make_pair("CELL+", new Function(cell_plus)));
    glossary.push_back(std::make_pair("FILL", new Function(fill)));
    glossary.push_back(std::make_pair("ERASE", new Function(erase)));
    glossary.push_back(std::make_pair("DUMP", new Function(dump)));
    glossary.push_back(std::make_pair("'", new Function(tick)));
    glossary.push_back(std::make_pair("EXECUTE", new Function(execute)));
    glossary.push_back(std::make_pair("PAGE", new Function(page)));
    glossary.push_back(std::make_pair("QUIT", new Function(quit)));
    glossary.push_back(std::make_pair("?STACK", new Function(stack_q)));
    glossary.push_back(std::make_pair("SP@", new Function(sp_at)));
    glossary.push_back(std::make_pair("TIB", new Function(tib)));
    glossary.push_back(std::make_pair("#TIB", new Function(pound_tib)));
    glossary.push_back(std::make_pair("SP0", new Var((int32_t) (sys.stack - 1), 4)));

    //Build the compile-time-only glossary
    comp_glossary.push_back(std::make_pair("EXIT", new Function(exit)));
    comp_glossary.push_back(std::make_pair("<#", new Function(bracket_pound)));
    comp_glossary.push_back(std::make_pair("#>", new Function(pound_bracket)));
    comp_glossary.push_back(std::make_pair("#", new Function(pound)));
    comp_glossary.push_back(std::make_pair("#S", new Function(pounds)));
    comp_glossary.push_back(std::make_pair("HOLD", new Function(hold)));
    comp_glossary.push_back(std::make_pair("SIGN", new Function(sign)));

    //Loop until terminal exit command is issued
    while(!BYE) {
        printf("#F> ");
        //ReadInput(stdin, 1024);

		fgets(sys.buf, 1024, stdin);
		sys.idx = sys.buf;
		sys.buf_len = strlen(sys.buf);
		for (int32_t i = 0; sys.idx[i]; i++)
			sys.idx[i] = toupper(sys.idx[i]);

        //Try and do something, and catch the abort if it throws
        try {
            text_interpreter();
        } catch(int32_t) {} //Abort catching
        //Newline printing and flag resetting
        if(S_UND) printf("stack empty");
        if(!BYE && !ABORT && !QUIT && !S_UND) printf("ok ");
        if(!BYE && !PAGE) printf("\n\n");
        if(QUIT) QUIT = false;
        if(ABORT) ABORT = false;
        if(S_UND) S_UND = false;
        if(PAGE) PAGE = false;
    }

    //Clean up the environment (no, not that kind, the other kind)
    for(auto itr = glossary.begin(); itr != glossary.end(); itr++)
        delete itr->second;

    return 0;
}

//UNSUPPORTED FORTH FEATURES:

//M*/ is not supported due to C/C++'s lack of support for 128 bit integers for 32 bit applications

//User variables pertaining to memory locations in the FORTH system are not supported,
//due to lacking meaningful return values

//BASE is not supported due to C++'s lack of support for printing signed integers in multiple bases,
//and lack of support for arbitrary-base printing in general
