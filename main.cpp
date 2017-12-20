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

//Proper header is used for each embeddable FORTH word to A: help with organization, and B: keep dependencies straight

//Output words
int32_t cr();
int32_t spaces();
int32_t space();
int32_t emit();
int32_t print();
int32_t uprint();
int32_t dprint();
int32_t urjprint();
int32_t drjprint();
int32_t printS();
int32_t type();

//String manip words
int32_t trailing();

//Print32_t formatting words
int32_t bracket_pound();
int32_t pound_bracket();
int32_t pound();
int32_t hold();
int32_t sign();

//Integer math words
int32_t add();
int32_t Dadd();
int32_t Madd();
int32_t sub();
int32_t Dsub();
int32_t mult();
int32_t umult();
int32_t Mmult();
int32_t div();
int32_t mod();
int32_t modDiv();
int32_t UmodDiv();
int32_t SmodDiv();
int32_t FmodDiv();
int32_t multDiv();
int32_t multDivMod();
int32_t add1();
int32_t sub1();
int32_t add2();
int32_t sub2();
int32_t abs();
int32_t dabs();
int32_t neg();
int32_t Dneg();
int32_t min();
int32_t Dmin();
int32_t max();
int32_t Dmax();
int32_t lshift();
int32_t rshift();

//Logic words
int32_t and_();
int32_t or_();
int32_t equals();
int32_t Dequals();
int32_t lessThan();
int32_t UlessThan();
int32_t DlessThan();
int32_t DUlessThan();
int32_t greaterThan();
int32_t zeroEquals();
int32_t DzeroEquals();
int32_t zeroLessThan();
int32_t zeroGreaterThan();

//Stack structure words
int32_t swap();
int32_t swap2();
int32_t dup();
int32_t dup2();
int32_t dup_if();
int32_t over();
int32_t over2();
int32_t rot();
int32_t drop();
int32_t drop2();
int32_t tuck();

//Return stack access words
int32_t retPush();
int32_t retPop();
int32_t retCopy();
int32_t retCopy3();

//Structural (branching) words
int32_t cond();
int32_t loop();
int32_t loop_plus();
int32_t do_();
int32_t nop();
int32_t leave();

//Variable/constant words
int32_t store();
int32_t store2();
int32_t c_store();
int32_t plus_store();
int32_t fetch();
int32_t fetch2();
int32_t c_fetch();
int32_t query();
int32_t cell();
int32_t cells();
int32_t cell_plus();
int32_t fill();
int32_t erase();
int32_t dump();

//Low-level memory access words
int32_t sp_at();
int32_t tib();
int32_t pound_tib();

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

//( -- )
//Prints a newline character to the terminal
int32_t cr() {
    printf("\n");
    return 0;
}

//( n -- )
//Prints n spaces
int32_t spaces() {
    std::string str((unsigned long)*(int32_t*)stack_at(0), ' ');
    printf("%s", str.c_str());
    stack_pop(1);
    return 0;
}

//( -- )
//Prints a space
int32_t space() {
    printf(" ");
    return 0;
}

//( c -- )
//Prints character c
int32_t emit() {
    char ch = (char)*stack_at(0);
    printf("%c", ch);
    stack_pop(1);
    return 0;
}

// ( n -- )
//Prints and pops the top of the stack, followed by a space
int32_t print() {
    printf("%d ", *(int32_t*)stack_at(0));
    stack_pop(1);
    return 0;
}

// ( u -- )
//Unsigned int32_t print
int32_t uprint() {
    printf("%u ", *stack_at(0));
    stack_pop(1);
    return 0;
}

// ( d -- )
//Double length integer print
int32_t dprint() {
    printf("%lld ", *(int64_t*)stack_at(0));
    stack_pop(2);
    return 0;
}
// ( u1 u2 -- )
//Unsigned right-justified print
int32_t urjprint() {
    uint32_t size = *stack_at(0);
    uint32_t data =  *stack_at(1);
    uint32_t num_spaces = size - (uint32_t) floor(log10((float)data));
    stack_pop(2);
    for(uint32_t i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%u", data);
    return 0;
}

// ( addr u -- )
int32_t type() {
    uint32_t u = *stack_at(0);
    char* str = (char*)*stack_at(1);
    stack_pop(2);
    fwrite(str, 1, u, stdout);
    return 0;
}

// ( addr u1 -- addr u2 )
int32_t trailing() {
    uint32_t u = *stack_at(0);
    char* str = (char*)*stack_at(1);
    stack_pop(1);
    //Creatively (ab)using for loops for fun and profit
    for(; str[u-1] != ' '; u--);
    str[u] = '\0';
    stack_push((int32_t) u);
    return 0;
}

// ( ud -- ud )
int32_t bracket_pound() {
    sys.swp = (char*) sys.stack + 256;
    sys.swp_len = 0;
    return 0;
}

// ( ud -- addr u )
int32_t pound_bracket() {
    stack_pop(2);
    stack_push((int32_t) sys.swp);
    stack_push((int32_t) sys.swp_len);
    return 0;
}

// ( ud1 -- ud2 )
int32_t pound() {
    uint64_t ud = *(uint64_t*)stack_at(1);
    memmove(sys.swp + 1, sys.swp, sys.swp_len);
    //ASCII table magic
    char digit = (ud % 10) + '0';
    sys.swp[0] = digit;
    *(uint64_t*)stack_at(1) /= 10;
    sys.swp_len++;
    return 0;
}

// ( ud1 -- 0 )
int32_t pounds() {
    uint64_t ud = *(uint64_t*)stack_at(1);
    char buf[32];
    uint32_t len = (uint32_t) sprintf(buf, "%llu", ud);
    memmove(sys.swp + len, sys.swp, sys.swp_len);
    memmove(sys.swp, buf, len);
    *(uint64_t*)stack_at(1) = 0;
    sys.swp_len += len;
    return 0;
}

// ( ud1 c -- ud1 )
int32_t hold() {
    memmove(sys.swp + 1, sys.swp, sys.swp_len);
    sys.swp[0] = *(char*)stack_at(0);
    stack_pop(1);
    sys.swp_len++;
    return 0;
}

// ( ud1 -- ud1 )
int32_t sign() {
    int32_t n = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(n < 0) {
        memmove(sys.swp + 1, sys.swp, sys.swp_len);
        sys.swp[0] = '-';
        sys.swp_len++;
    }
    return 0;
}

// ( n1 n2 -- sum )
//Adds n2 to n1
int32_t add() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) += s;
    return 0;
}

// ( d1 d2 -- sum )
// Adds d2 to d1
int32_t Dadd() {
    int64_t s = *(int64_t*)stack_at(0);
    stack_pop(2);
    *(int64_t*)stack_at(0) += s;
    return 0;
}

// ( d n -- sum )
// Adds n to d - double length result
int32_t Madd() {
    int32_t n = *(int32_t*)stack_at(0);
    stack_pop(2);
    *(int64_t*)stack_at(0) += (int64_t)n;
    return 0;
}

// ( n1 n2 -- diff )
// Subtracts n2 from n1
int32_t sub() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) -= s;
    return 0;
}

// ( d1 d2 -- diff )
// Subtracts d2 from d1
int32_t Dsub() {
    int64_t s = *(int64_t*)stack_at(0);
    stack_pop(2);
    *(int64_t*)stack_at(0) -= s;
    return 0;
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2
int32_t mult() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) *= s;
    return 0;
}

// ( u1 u2 -- prod )
// Multiplies u1 by u2
int32_t umult() {
    int32_t s = *stack_at(0);
    int32_t m = *stack_at(1);
    stack_pop(2);
    stack_push((int64_t)s*m);
    return 0;
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2 - double length result
int32_t Mmult() {
    int32_t s = *(int32_t*)stack_at(0);
    int32_t m = *(int32_t*)stack_at(1);
    stack_pop(2);
    stack_push((int64_t)s*m);
    return 0;
}

// ( n1 n2 -- qout )
// Divides n1 by n2
int32_t div() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) /= s;
    return 0;
}

// ( n1 n2 -- rem )
// Computes n1 mod n2
int32_t mod() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) %= s;
    return 0;
}

// ( n1 n2 -- rem quot )
// Divides n1 by n2, giving remainder and quotient
int32_t modDiv() {
    int32_t m, s = *(int32_t*)stack_at(0);
    stack_pop(1);
    m = *(int32_t*)stack_at(0) / s;
    *(int32_t*)stack_at(0) %= s;
    stack_push(m);
    return 0;
}

// ( u1 u2 -- quot )
// Divides u1 by u2
int32_t UmodDiv() {
    uint32_t m, s = *stack_at(0);
    stack_pop(1);
    m = *stack_at(0) % s;
    *stack_at(0) /= s;
    //Pointer hackery to get an unsigned int32_t pushed onto the stack
    stack_push(*(int32_t*)&m);
    return 0;
}

// ( d n1  -- n2 n3 )
// Divides d by n1, giving symmetric quotient n3 and remainder n2
int32_t SmodDiv() {
    int32_t n = *(int32_t*)stack_at(0);
    int64_t s = *(int64_t*)stack_at(1);
    stack_pop(3);
    stack_push((int32_t) s % n);
    stack_push((int32_t) round((float) s / n ));
    return 0;
}

// ( d n1 -- n2 n3 )
// Divides d by n1, giving floored quotient n3 and remainder n2
int32_t FmodDiv() {
    int32_t n = *(int32_t*)stack_at(0);
    int64_t s = *(int64_t*)stack_at(1);
    stack_pop(3);
    stack_push((int32_t) s % n);
    stack_push((int32_t) s / n);
    return 0;
}

// ( n1 n2 n3 -- n-result )
// Multiplies n1 by n2, then divides by n3. Uses a double length intermediate.
int32_t multDiv(){
    int64_t a = *(int32_t*)stack_at(2), b = *(int32_t*)stack_at(1), c = *(int32_t*)stack_at(0);
    stack_pop(3);
    int64_t m = a * b;
    int32_t d = (int32_t)(m / c);
    stack_push(d);
    return 0;
}

// ( n1 n2 n3 -- n-rem, n-result)
// Multiplies n1 by n2, then divides by n3. Returns the quotient and remainder. Uses a double-length intermediate.
int32_t multDivMod(){
    long a = *(int32_t*)stack_at(2), b = *(int32_t*)stack_at(1), c = *(int32_t*)stack_at(0);
    stack_pop(3);
    long m = a * b;
    int32_t d = (int32_t)(m / c), r = (int32_t)(m % c);
    stack_push(r);
    stack_push(d);
    return 0;
}

// ( n1 -- n2 )
//Increments the top of the stack
int32_t add1() {
    (*(int32_t*)stack_at(0))++;
    return 0;
}

// ( n1 -- n2 )
//Decrements the top of the stack
int32_t sub1() {
    (*(int32_t*)stack_at(0))--;
    return 0;
}

// ( n1 -- n2 )
//Adds 2 to the top of the stack
int32_t add2() {
    *(int32_t*)stack_at(0) += 2;
    return 0;
}

// ( n1 -- n2 )
//Subtracts 2 from the top of the stack
int32_t sub2() {
    *(int32_t*)stack_at(0) -= 2;
    return 0;
}

// ( n1 -- n2 )
//Leftshifts the top of the stack by 1
int32_t lshift() {
    *(int32_t*)stack_at(0) *= 2;
    return 0;
}

// ( n1 -- n2 )
//Rightshifts the top of the stack by 1
int32_t rshift() {
    *(int32_t*)stack_at(0) /= 2;
    return 0;
}

// ( f1 f2 -- f3 )
//Binary and operator
int32_t and_() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) &= s;
    return 0;
}

// ( f1 f2 -- f3 )
//Binary or operator
int32_t or_() {
    int32_t s = *(int32_t*)stack_at(0);
    stack_pop(1);
    *(int32_t*)stack_at(0) |= s;
    return 0;
}

// ( n -- u)
//Computes absolute value of the top of the stack
int32_t abs(){
    *(int32_t*)stack_at(0) = std::abs(*(int32_t*)stack_at(0));
    return 0;
}

// ( d -- ud )
int32_t dabs() {
    *(int64_t*)stack_at(0) = std::abs(*(int64_t*)stack_at(0));
    return 0;
}

// ( n1 -- n2 )
//Negates the top of the stack
int32_t neg(){
    *(int32_t*)stack_at(0) *= -1;
    return 0;
}

// ( d1 -- d2 )
//Negates the double-length top of the stack
int32_t Dneg() {
    *(int64_t*)stack_at(0) *= -1;
    return 0;
}

// ( n1 n2 -- n3 )
//Returns minimum of n1 and n2
int32_t min(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    int32_t b = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(a > b)
        stack_push(b);
    else
        stack_push(a);
    return 0;
}

// ( d1 d2 -- d3 )
// Returns the minimum of 2 double-length numbers
int32_t Dmin(){
    int64_t a = *(int64_t*)stack_at(0);
    stack_pop(2);
    int64_t b = *(int64_t*)stack_at(0);
    stack_pop(2);
    if(a > b)
        stack_push(b);
    else
        stack_push(a);
    return 0;
}

// ( n1 n2 -- n3 )
//Returns maximum of 2 numbers
int32_t max(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    int32_t b = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(a > b)
        stack_push(a);
    else
        stack_push(b);
    return 0;
}

// ( d1 d2 -- d3 )
// Returns the maximum of 2 double-length numbers
int32_t Dmax(){
    int64_t a = *(int64_t*)stack_at(0);
    stack_pop(2);
    int64_t b = *(int64_t*)stack_at(0);
    stack_pop(2);
    if(a > b)
        stack_push(a);
    else
        stack_push(b);
    return 0;
}

// ( n1 n2 -- n2 n1 )
//Swaps top two elements of the stack
int32_t swap() {
    int32_t t = *stack_at(0);
    stack_pop(1);
    int32_t b = *stack_at(0);
    stack_pop(1);
    stack_push(t);
    stack_push(b);
    return 0;
}

// ( d1 d2 -- d2 d1 )
//Swaps top two elements of the stack for the next two
int32_t swap2() {
    int64_t t = *(int64_t*)stack_at(0);
    stack_pop(2);
    int64_t b = *(int64_t*)stack_at(0);
    stack_pop(2);
    stack_push(t);
    stack_push(b);
    return 0;
}

// ( n -- n n )
//Duplicates the top of the stack
int32_t dup() {
    stack_push(*(int32_t*)stack_at(0));
    return 0;
}

// ( d -- d d )
//Duplicates the top two elements of the stack
int32_t dup2() {
    stack_push(*(int64_t*)stack_at(0));
    return 0;
}

//  ( f -- f f )
// Duplicates the top of the stack if it isn't 0
int32_t dup_if() {
    int32_t q = *(int32_t*)stack_at(0);
    if(q) stack_push(q);
    return 0;
}

// ( n1 n2 -- n1 n2 n1 )
//Pushes the second element of the stack onto the stack
int32_t over() {
    int32_t t = *stack_at(0);
    stack_pop(1);
    int32_t d = *stack_at(0);
    stack_push(t);
    stack_push(d);
    return 0;
}

// ( d1 d2 -- d1 d2 d1 )
//Pushes the third and fourth elements of the stack onto the stack
int32_t over2() {
    int64_t t = *(int64_t*)stack_at(0);
    stack_push(t);
    return 0;
}

// ( n1 n2 n3 -- n2 n3 n1 )
//Removes the third element of the stack and pushes it onto the stack
int32_t rot() {
    int32_t t = *stack_at(0);
    stack_pop(1);
    int32_t m = *stack_at(0);
    stack_pop(1);
    int32_t b = *stack_at(0);
    stack_pop(1);
    stack_push(m);
    stack_push(t);
    stack_push(b);
    return 0;
}

// ( n -- )
//Pops the top of the stack
int32_t drop() {
    stack_pop(1);
    return 0;
}

// ( d -- )
//Pops the top 2 elements of the stack
int32_t drop2() {
    stack_pop(2);
    return 0;
}

// ( n1 n2 -- n2 n1 n2 )
int32_t tuck() {
    int32_t n1 = *(int32_t*)stack_at(0);
    int32_t n2 = *(int32_t*)stack_at(1);
    stack_pop(2);
    stack_push(n1);
    stack_push(n2);
    stack_push(n1);
    return 0;
}

// ( n -- )     rstack: ( -- n )
//Pushes top of stack onto return stack
int32_t retPush(){
    int32_t a = *stack_at(0);
    stack_pop(1);
    rstack_push(a);
    return 0;
}

// ( -- n )     rstack: ( n -- )
//Pushes top of return stack onto stack
int32_t retPop(){
    int32_t a = *rstack_at(0);
    rstack_pop(1);
    stack_push(a);
    return 0;
}

// ( -- n)      rstack: ( -- )
//Copies top of return stack onto stack
int32_t retCopy(){
    int32_t a = *rstack_at(0);
    stack_push(a);
    return 0;
}

// ( -- n )     rstack: ( -- )
//Copies 3rd value on return stack onto stack
int32_t retCopy3(){
    int32_t a = *rstack_at(2);
    stack_push(a);
    return 0;
}

// ( d u -- )
// Double-length right-justified print
int32_t drjprint() {
    uint32_t size = *stack_at(0);
    int64_t data = *(int64_t*)stack_at(1);
    int64_t num_spaces = size - (int64_t) floor(log10((float)data));
    stack_pop(3);
    for(int64_t i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%lld ", data);
    return 0;
}

// ( -- )
//Prints the contents of the stack
int32_t printS() {
    for(uint32_t* a = sys.stack; a != sys.stack_0; a++) {
        printf("%d ", *(int32_t*) a);
    }
    return 0;
}

// ( n1 n2 -- f )
// Compares the two numbers at the top of the stack for equality
int32_t equals(){
    int32_t a = *(int32_t*)stack_at(0), b = *(int32_t*)stack_at(1);
    stack_pop(2);
    if(a == b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
//Compares two double-length numbers at the top of the stack for equality
int32_t Dequals() {
    int64_t a = *(int64_t*)stack_at(0), b = *(int64_t*)stack_at(2);
    stack_pop(4);
    if(a == b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n1 n2 -- f )
int32_t lessThan(){
    int32_t a = *(int32_t*)stack_at(1), b = *(int32_t*)stack_at(0);
    stack_pop(2);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( u1 u2 -- f )
int32_t UlessThan(){
    uint32_t a = *stack_at(1), b = *stack_at(0);
    stack_pop(2);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
int32_t DlessThan() {
    int64_t a = *(int64_t*)stack_at(0), b = *(int64_t*)stack_at(2);
    stack_pop(4);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
int32_t DUlessThan() {
    uint64_t a = *(uint64_t*)stack_at(1), b = *(uint64_t*)stack_at(3);
    stack_pop(4);
    if(a < b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n1 n2 -- f )
int32_t greaterThan(){
    int32_t a = *(int32_t*)stack_at(1), b = *(int32_t*)stack_at(0);
    stack_pop(2);
    if(a > b)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is equal to zero
int32_t zeroEquals(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(a == 0)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( d -- f )
// Tests whether the double-length top of the stack is equal to zero
int32_t DzeroEquals(){
    int64_t a = *(int64_t*)stack_at(0);
    stack_pop(2);
    if(a == 0)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is less than zero
int32_t zeroLessThan(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(0 < a)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is greater than zero
int32_t zeroGreaterThan(){
    int32_t a = *(int32_t*)stack_at(0);
    stack_pop(1);
    if(0 > a)
        stack_push((int32_t)0xffffffff);
    else
        stack_push(0x00000000);
    return 0;
}

// ( n addr -- )
// Stores n at the memory location pointed to by addr
int32_t store() {
    int* ptr = (int32_t*)*stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    stack_pop(2);
    *ptr = n;
    return 0;
}

// ( d addr -- )
// Stores d at the memory location pointed to by addr
int32_t store2() {
    int64_t* ptr = (int64_t*)*stack_at(0);
    int64_t n = *(int64_t*)stack_at(2);
    stack_pop(3);
    *ptr = n;
    return 0;
}

// ( c addr -- )
// Stores c at the memory location pointed to by addr
int32_t c_store() {
    char *ptr = (char *) *stack_at(0);
    char n = *(char *) stack_at(1);
    stack_pop(2);
    *ptr = n;
    return 0;
}

// ( n addr -- )
// Adds n to the number stored at addr
int32_t plus_store() {
    int* ptr = (int32_t*)*stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    stack_pop(2);
    *ptr += n;
    return 0;
}

// ( addr -- n )
// Pushes the contents of addr to the top of the stack
int32_t fetch() {
    int* ptr = (int32_t*)*stack_at(0);
    stack_pop(1);
    stack_push(*ptr);
    return 0;
}

// ( addr -- d )
// Pushes the double-length contents of addr to the top of the stack
int32_t fetch2() {
    int64_t* ptr = (int64_t*)*stack_at(0);
    stack_pop(1);
    stack_push(*ptr);
    return 0;
}

// ( addr -- c )
// Pushes the byte pointed to by addr to the top of the stack
int32_t c_fetch() {
    char* ptr = (char*)*stack_at(0);
    stack_pop(1);
    stack_push((int32_t)*ptr);
    return 0;
}

// ( addr  -- n )
// Equivalent to @ .
int32_t query() {
    fetch();
    print();
    return 0;
}

// ( -- 4 )
// Pushes 4 (the size of a cell) onto the stack
int32_t cell() {
    stack_push(4);
    return 0;
}

// ( n1 -- n2 )
// Multiplies n1 by 4
int32_t cells() {
    *stack_at(0) *= 4;
    return 0;
}

// ( addr1 -- addr2 )
// Increments addr1 by the size of a cell
int32_t cell_plus() {
    cell();
    add();
    return 0;
}

// ( n c addr -- )
// Fills n1 bytes at addr with byte c
int32_t fill() {
    char b = *(char*)stack_at(0);
    int32_t n = *(int32_t*)stack_at(1);
    char* addr = (char*)*stack_at(2);
    stack_pop(3);
    for(int32_t i = 0; i < n; i++)
        addr[i] = b;
    return 0;
}

// ( n addr -- )
// Fills n bytes at addr with 0
int32_t erase() {
    stack_push(0);
    return fill();
}

// ( u addr -- )
// Prints the contents of u bytes at addr
int32_t dump() {
    uint32_t c = *stack_at(0);
    int* addr = (int32_t*)stack_at(1);
    stack_pop(2);
    for(uint32_t i = 0; i < c; i++)
        if(i % 4 == 0) printf("%d ", addr[i / 4]);
    return 0;
}

// ( -- addr )
//Pushes the address of the top of the stack onto the stack_q
int32_t sp_at() {
    stack_push((int32_t) stack_at(0));
    return 0;
}

// ( -- addr )
//Pushes the address of the start of the trminal input buffer onto the stack
int32_t tib() {
    stack_push((int32_t) sys.buf);
    return 0;
}

// ( -- u )
//Pushes the a pointer to the length of the terminal input buffer onto the stack
int32_t pound_tib() {
    stack_push((int32_t) &sys.buf_len);
    return 0;
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
