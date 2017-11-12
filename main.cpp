// C imports
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <stdint.h>

// C++ imports
#include <vector>
#include <list>
#include <stack>
#include <unordered_map>

// .h file imports
#include "Stack.h"
#include "Function.h"

//Global file stream index pointer
char* idx;

//Utility macro for getting char-delimited substrings of C strings
#define GetSubstring(boolean) tmp_idx = idx; for (i = 0; !(boolean); i++) tmp_idx++; \
                           tmp_buf = (char *) malloc(i + 1); for (size_t j = 0; j < i; j++) tmp_buf[j] = idx[j]; \
                           tmp_buf[i] = 0; idx = tmp_idx; if (*idx) idx++

//Utiity macro for reading from a file/stream
//I may remove this later, as it's only used in 1 place
#define ReadInput(file) char* buf = NULL; size_t n = 0; getline(&buf, &n, file); idx = buf; \
                        for(int i = 0; idx[i]; i++) idx[i] = toupper(idx[i])

//Utility macro for variable creation
#define MakeVar(type) GetSubstring(isspace(*tmp_idx)); Function *f = new Function(nop); f->next = new Function*[1]; \
                      f->next[0] = new type; glossary.push_back(std::make_pair(tmp_buf, f))

//Global boolean flags for program state management
bool ABORT = false, BYE = false, QUIT = false, S_UND = false, PAGE = false;

Stack *stack, *return_stack;

std::list<std::pair<std::string, Function*> > glossary;

//Proper header is used for each embeddable FORTH word to A: help with organization, and B: keep dependencies straight

//Output words
int cr();
int spaces();
int space();
int emit();
int print();
int uprint();
int dprint();
int urjprint();
int drjprint();
int printS();

//Integer math words
int add();
int Dadd();
int Madd();
int sub();
int Dsub();
int mult();
int umult();
int Mmult();
int div();
int mod();
int modDiv();
int UmodDiv();
int SmodDiv();
int FmodDiv();
int multDiv();
int multDivMod();
int add1();
int sub1();
int add2();
int sub2();
int abs();
int neg();
int Dneg();
int min();
int Dmin();
int max();
int Dmax();

//Bithacking words
int lshift();
int rshift();

//Logic words
int and_();
int or_();
int equals();
int Dequals();
int lessThan();
int UlessThan();
int DlessThan();
int DUlessThan();
int greaterThan();
int zeroEquals();
int DzeroEquals();
int zeroLessThan();
int zeroGreaterThan();

//Stack structure words
int swap();
int swap2();
int dup();
int dup2();
int dup_if();
int over();
int over2();
int rot();
int drop();
int drop2();

//Return stack access words
int retPush();
int retPop();
int retCopy();
int retCopy3();

//Structural (branching) words
int cond();
int loop();
int loop_plus();
int do_();
int nop();
int leave();

//Variable/constant words
int store();
int store2();
int c_store();
int plus_store();
int fetch();
int fetch2();
int c_fetch();
int query();
int cells();
int fill();
int erase();
int dump();

//Reflectine words
int tick();
int execute();

//Misc. words
int page();
int quit();
int abort_();
int stack_q();


void number(std::string& str);

//Finds words in the glossary
Function* find(std::string& name) {
    for(auto itr = glossary.rbegin(); itr != glossary.rend(); itr++) {
        if(itr->first == name)
            return itr->second;
    }
    return NULL;
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

    if(itr != glossary.end())
        glossary.erase(itr, glossary.end());
}

//Adds user-defined words to the glossary

//WARNING: This code is very obtuse, despite the extensive comments. I shouldn't have used so much C-style code,
// but what's done is done. I could change to using vectors, and it might make a little bit more sense, but probably not.
//Graphs are rarely clean to implement, especially due to how I built the path decider into everything. It makes for a clean
//path decider, but messy graph handling.
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
            tail->next = new Function *[1];
            tail->next[0] = node;
            tail = node;
        } else if(func == "ABORT\"") {                   //Handles ABORT"
            GetSubstring(*tmp_idx == '"');
            Abort *node = new Abort(tmp_buf);
            tail->next = new Function *[1];
            tail->next[0] = node;
            tail = node;
        } else if(func == "IF") {                        //Conditional handling, step 1
            Function* if_ = new Function(cond);          //Allcoate & initialize the if node
            tail->next = new Function*[1];
            tail->next[0] = if_;                         //Tail points to if
            tail->next[0]->next = new Function*[2];      //if block branching nodes declaration
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
            tail->next = new Function*[1];               //Stitch the active tail into the dummy node
            tail->next[0] = then;
            if(!if_stack.top()->next)                    //Handle possible if/else
                if_stack.top()->next = new Function*[1];
            if_stack.top()->next[0] = then;              //Stitch the other tail into the dummy node
            tail = tail->next[0];
            if_stack.pop();                              //Clean up the conditional stack
        } else if(func == "THEN") {
            Function* then = new Function(nop);          //Strucutral merger node allocation
            tail->next = new Function*[1];
            tail->next[0] = then;                        //Tie the tail into the node
            if(!if_stack.top()->next)                    //Make sure the if-branch can be tied into the node
                if_stack.top()->next = new Function*[1];
            if_stack.top()->next[0] = then;              //Tie the if_stack tail into the node
            tail = tail->next[0];
            if_stack.pop();
        } else if(func == "DO") {
            Function *_do = new Function(do_);           //Allocate do function
            tail->next = new Function *[1];
            tail->next[0] = _do;                         //Set tail->next
            tail = tail->next[0];                        //Move tail
            Function *loop_head = new Function(nop);     //Allocate loop block head
            tail->next = new Function *[1];
            tail->next[0] = loop_head;                   //Move tail to loop head
            do_stack.push(loop_head);                    //Put head on do loop stack
            tail = tail->next[0];
            std::vector<Function *> temp;
            leave_stack.push(temp);                      //Set up leave stack
        } else if(func == "LEAVE") {
            Function *_leave = new Function(leave);      //Allocate leave node
            tail->next = new Function*[1];
            tail->next[0] = _leave;                      //Set tail->next
            leave_stack.top().push_back(_leave);         //Push node onto leave stack
            tail = tail->next[0];
            tail->next = new Function*[2];               //Set up leave escape path
            tail->next[0] = new Function(nop);
            tail = tail->next[0];
        } else if(func == "LOOP" || func == "+LOOP") {
            Function *loop_;
            if (func == "LOOP")
                loop_ = new Function(loop);              //Allocate loop escape checker function
            else
                loop_ = new Function(loop_plus);         //Differentiate between LOOP and +LOOP
            tail->next = new Function*[1];
            tail->next[0] = loop_;                       //Add loop escape checker to loop path
            loop_->next = new Function*[2];              //Allocate loop branching
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
            tail->next = new Function*[1];
            tail->next[0] = begin;                       //Set tail->next
            begin_stack.push(begin);                     //Push loop head onto stack
            tail = tail->next[0];                        //Move tail
        } else if (func == "UNTIL") {
            Function* until = new Function(cond);        //Allocate until conditional
            tail->next = new Function*[1];
            tail->next[0] = until;                       //Set tail->next
            tail = tail->next[0];                        //Move tail
            tail->next = new Function*[2];
            tail->next[0] = begin_stack.top();           //Point the conditional false path at loop head
            begin_stack.pop();                           //Clean up loop stack
            Function* temp = new Function(nop);          //Set up escape tail
            tail->next[1] = temp;
            tail = tail->next[1];
        } else if (func == "WHILE") {
            Function* while_ = new Function(cond);       //Allocate while conditional
            tail->next = new Function*[1];
            tail->next[0] = while_;                      //Set tail->next
            tail = tail->next[0];                        //Move tail->next
            tail->next = new Function*[2];               //Allocate branching
            Function* temp = new Function(nop);
            tail->next[1] = temp;                        //Set up true path
            temp = new Function(nop);
            tail->next[0] = temp;                        //Set up false path
            while_stack.push(temp);                      //Push false path head onto while stack
            tail = tail->next[1];                        //Move tail
        } else if (func == "REPEAT") {
            tail->next = new Function *[1];
            tail->next[0] = begin_stack.top();           //Plug current tail into loop head
            begin_stack.pop();                           //Clean up begin_stack
            tail = while_stack.top();                    //Move current tail to loop escape path
            while_stack.pop();                           //Clean up while_stack
        } else if (func == name) {                       //Allow for recursive calls
            tail->next = new Function*[1];
            tail->next[0] = (Function*) new UsrFunc(head);
            tail = tail->next[0];
        } else {
            Function *temp;
            Function *tmp_ptr = find(func);
            //Figure out what the heck kind of thing we're dealing with
            temp = (Function *) (tmp_ptr ? (tmp_ptr->next ? new UsrFunc(tmp_ptr) : new Function(tmp_ptr)) : new Number(func));
            tail->next = new Function *[1];
            tail->next[0] = temp;
            tail = tail->next[0];
        }
        while (isspace(*idx)) idx++;                        //Take care of loose/excess whitespace
        GetSubstring(isspace(*tmp_idx));
        func = std::string(tmp_buf);
    }
    glossary.push_back(std::make_pair(name, head));
}

//Word-execution wrapper - executed word pointed to by func
//Written to help debugging and to avoid stack overflow resulting from excessive recursion
void run(Function* func) {
    while(func->next) {
        int idx = func->run();
        func = func->next[idx];
    }
    func->run();
}

//( -- )
//Prints a newline character to the terminal
int cr() {
    printf("\n");
    return 0;
}

//( n -- )
//Prints n spaces
int spaces() {
    std::string str((unsigned long)*(int*)stack->at(0), ' ');
    printf(str.c_str());
    stack->pop(1);
    return 0;
}

//( -- )
//Prints a space
int space() {
    printf(" ");
    return 0;
}

//( c -- )
//Prints character c
int emit() {
    char ch = (char)*stack->at(0);
    printf("%c", ch);
    stack->pop(1);
    return 0;
}

// ( n1 n2 -- sum )
//Adds n2 to n1
int add() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) += s;
    return 0;
}

// ( d1 d2 -- sum )
// Adds d2 to d1
int Dadd() {
    int64_t s = *(int64_t*)stack->at(1);
    stack->pop(2);
    *(int64_t*)stack->at(1) += s;
    return 0;
}

// ( d n -- sum )
// Adds n to d - double length result
int Madd() {
    int n = *(int*)stack->at(0);
    stack->pop(2);
    *(int64_t*)stack->at(1) += (int64_t)n;
    return 0;
}

// ( n1 n2 -- diff )
// Subtracts n2 from n1
int sub() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) -= s;
    return 0;
}

// ( d1 d2 -- diff )
// Subtracts d2 from d1
int Dsub() {
    int64_t s = *(int64_t*)stack->at(1);
    stack->pop(2);
    *(int64_t*)stack->at(1) -= s;
    return 0;
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2
int mult() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) *= s;
    return 0;
}

// ( u1 u2 -- prod )
// Multiplies u1 by u2
int umult() {
    int s = *stack->at(0);
    int m = *stack->at(1);
    stack->pop(2);
    stack->push((int64_t)s*m);
    return 0;
}

// ( n1 n2 -- prod )
// Multiplies n1 by n2 - double length result
int Mmult() {
    int s = *(int*)stack->at(0);
    int m = *(int*)stack->at(1);
    stack->pop(2);
    stack->push((int64_t)s*m);
    return 0;
}

// ( n1 n2 -- qout )
// Divides n1 by n2
int div() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) /= s;
    return 0;
}

// ( n1 n2 -- rem )
// Computes n1 mod n2
int mod() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) %= s;
    return 0;
}

// ( n1 n2 -- rem quot )
// Divides n1 by n2, giving remainder and quotient
int modDiv() {
    int m, s = *(int*)stack->at(0);
    stack->pop(1);
    m = *(int*)stack->at(0) / s;
    *(int*)stack->at(0) %= s;
    stack->push(m);
    return 0;
}

// ( u1 u2 -- quot )
// Divides u1 by u2
int UmodDiv() {
    uint m, s = *stack->at(0);
    stack->pop(1);
    m = *stack->at(0) % s;
    *stack->at(0) /= s;
    //Pointer hackery to get an unsigned int pushed onto the stack
    stack->push(*(int*)&m);
    return 0;
}

// ( d n1  -- n2 n3 )
// Divides d by n1, giving symmetric quotient n3 and remainder n2
int SmodDiv() {
    int n = *(int*)stack->at(0);
    int64_t s = *(int64_t*)stack->at(2);
    stack->pop(3);
    stack->push((int) s % n);
    stack->push((int) round((float) s / n ));
    return 0;
}

// ( d n1 -- n2 n3 )
// Divides d by n1, giving floored quotient n3 and remainder n2
int FmodDiv() {
    int n = *(int*)stack->at(0);
    int64_t s = *(int64_t*)stack->at(2);
    stack->pop(3);
    stack->push((int) s % n);
    stack->push((int) s / n);
    return 0;
}

// ( n1 n2 n3 -- n-result )
// Multiplies n1 by n2, then divides by n3. Uses a double length intermediate.
int multDiv(){
    int64_t a = *(int*)stack->at(2), b = *(int*)stack->at(1), c = *(int*)stack->at(0);
    stack->pop(3);
    int64_t m = a * b;
    int d = (int)(m / c);
    stack->push(d);
    return 0;
}

// ( n1 n2 n3 -- n-rem, n-result)
// Multiplies n1 by n2, then divides by n3. Returns the quotient and remainder. Uses a double-length intermediate.
int multDivMod(){
    long a = *(int*)stack->at(2), b = *(int*)stack->at(1), c = *(int*)stack->at(0);
    stack->pop(3);
    long m = a * b;
    int d = (int)(m / c), r = (int)(m % c);
    stack->push(r);
    stack->push(d);
    return 0;
}

// ( n1 -- n2 )
//Increments the top of the stack
int add1() {
    (*(int*)stack->at(0))++;
    return 0;
}

// ( n1 -- n2 )
//Decrements the top of the stack
int sub1() {
    (*(int*)stack->at(0))--;
    return 0;
}

// ( n1 -- n2 )
//Adds 2 to the top of the stack
int add2() {
    *(int*)stack->at(0) += 2;
    return 0;
}

// ( n1 -- n2 )
//Subtracts 2 from the top of the stack
int sub2() {
    *(int*)stack->at(0) -= 2;
    return 0;
}

// ( n1 -- n2 )
//Leftshifts the top of the stack by 1
int lshift() {
    *(int*)stack->at(0) *= 2;
    return 0;
}

// ( n1 -- n2 )
//Rightshifts the top of the stack by 1
int rshift() {
    *(int*)stack->at(0) /= 2;
    return 0;
}

// ( f1 f2 -- f3 )
//Binary and operator
int and_() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) &= s;
    return 0;
}

// ( f1 f2 -- f3 )
//Binary or operator
int or_() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) |= s;
    return 0;
}

// ( n -- u)
//Computes absolute value of the top of the stack
int abs(){
    *(int*)stack->at(0) = std::abs(*(int*)stack->at(0));
    return 0;
}

// ( n1 -- n2 )
//Negates the top of the stack
int neg(){
    *(int*)stack->at(0) *= -1;
    return 0;
}

// ( d1 -- d2 )
//Negates the double-length top of the stack
int Dneg() {
    *(int64_t*)stack->at(1) *= -1;
    return 0;
}

// ( n1 n2 -- n3 )
//Returns minimum of n1 and n2
int min(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    int b = *(int*)stack->at(0);
    stack->pop(1);
    if(a > b)
        stack->push(b);
    else
        stack->push(a);
    return 0;
}

// ( d1 d2 -- d3 )
// Returns the minimum of 2 double-length numbers
int Dmin(){
    int64_t a = *(int64_t*)stack->at(1);
    stack->pop(2);
    int64_t b = *(int64_t*)stack->at(1);
    stack->pop(2);
    if(a > b)
        stack->push(b);
    else
        stack->push(a);
    return 0;
}

// ( n1 n2 -- n3 )
//Returns maximum of 2 numbers
int max(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    int b = *(int*)stack->at(0);
    stack->pop(1);
    if(a > b)
        stack->push(a);
    else
        stack->push(b);
    return 0;
}

// ( d1 d2 -- d3 )
// Returns the maximum of 2 double-length numbers
int Dmax(){
    int64_t a = *(int64_t*)stack->at(1);
    stack->pop(2);
    int64_t b = *(int64_t*)stack->at(1);
    stack->pop(2);
    if(a > b)
        stack->push(a);
    else
        stack->push(b);
    return 0;
}

// ( n1 n2 -- n2 n1 )
//Swaps top two elements of the stack
int swap() {
    int t = *stack->at(0);
    stack->pop(1);
    int b = *stack->at(0);
    stack->pop(1);
    stack->push(t);
    stack->push(b);
    return 0;
}

// ( d1 d2 -- d2 d1 )
//Swaps top two elements of the stack for the next two
int swap2() {
    int64_t t = *(int64_t*)stack->at(1);
    stack->pop(2);
    int64_t b = *(int64_t*)stack->at(1);
    stack->pop(2);
    stack->push(t);
    stack->push(b);
    return 0;
}

// ( n -- n n )
//Duplicates the top of the stack
int dup() {
    stack->push(*(int*)stack->at(0));
    return 0;
}

// ( d -- d d )
//Duplicates the top two elements of the stack
int dup2() {
    stack->push(*(int64_t*)stack->at(1));
    return 0;
}

//  ( f -- f f )
// Duplicates the top of the stack if it isn't 0
int dup_if() {
    int q = *(int*)stack->at(0);
    if(q) stack->push(q);
    return 0;
}

// ( n1 n2 -- n1 n2 n1 )
//Pushes the second element of the stack onto the stack
int over() {
    int t = *stack->at(0);
    stack->pop(1);
    int d = *stack->at(0);
    stack->push(t);
    stack->push(d);
    return 0;
}

// ( d1 d2 -- d1 d2 d1 )
//Pushes the third and fourth elements of the stack onto the stack
int over2() {
    int64_t t = *(int64_t*)stack->at(1);
    stack->push(t);
    return 0;
}

// ( n1 n2 n3 -- n2 n3 n1 )
//Removes the third element of the stack and pushes it onto the stack
int rot() {
    int t = *stack->at(0);
    stack->pop(1);
    int m = *stack->at(0);
    stack->pop(1);
    int b = *stack->at(0);
    stack->pop(1);
    stack->push(m);
    stack->push(t);
    stack->push(b);
    return 0;
}

// ( n -- )
//Pops the top of the stack
int drop() {
    stack->pop(1);
    return 0;
}

// ( d -- )
//Pops the top 2 elements of the stack
int drop2() {
    stack->pop(2);
    return 0;
}

// ( n -- )     rstack: ( -- n )
//Pushes top of stack onto return stack
int retPush(){
    int a = *stack->at(0);
    stack->pop(1);
    return_stack->push(a);
    return 0;
}

// ( -- n )     rstack: ( n -- )
//Pushes top of return stack onto stack
int retPop(){
    int a = *return_stack->at(0);
    return_stack->pop(1);
    stack->push(a);
    return 0;
}

// ( -- n)      rstack: ( -- )
//Copies top of return stack onto stack
int retCopy(){
    int a = *return_stack->at(0);
    stack->push(a);
    return 0;
}

// ( -- n )     rstack: ( -- )
//Copies 3rd value on return stack onto stack
int retCopy3(){
    int a = *return_stack->at(2);
    stack->push(a);
    return 0;
}

// ( n -- )
//Prints and pops the top of the stack, followed by a space
int print() {
    printf("%d ", *(int*)stack->at(0));
    stack->pop(1);
    return 0;
}

// ( u -- )
//Unsigned int print
int uprint() {
    printf("%u ", *stack->at(0));
    stack->pop(1);
    return 0;
}

// ( d -- )
//Double length integer print
int dprint() {
    printf("%lld ", *(int64_t*)stack->at(1));
    stack->pop(2);
    return 0;
}
// ( u1 u2 -- )
//Unsigned right-justified print
int urjprint() {
    uint size = *stack->at(0);
    uint data =  *stack->at(1);
    uint num_spaces = size - (uint) floor(log10((float)data));
    stack->pop(2);
    for(uint i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%u", data);
    return 0;
}

// ( d u -- )
// Double-length right-justified print
int drjprint() {
    uint size = *stack->at(0);
    int64_t data = *(int64_t*)stack->at(2);
    int64_t num_spaces = size - (int64_t) floor(log10((float)data));
    stack->pop(3);
    for(int64_t i = 0; i < num_spaces; i++)
        printf(" ");
    printf("%lld ", data);
    return 0;
}

// ( -- )
//Prints the contents of the stack
int printS() {
    for(int a = 0; a < stack->size(); a++) {
        printf("%d ", *(int*)stack->at(a));
    }
    return 0;
}

// ( n1 n2 -- f )
// Compares the two numbers at the top of the stack for equality
int equals(){
    int a = *(int*)stack->at(0), b = *(int*)stack->at(1);
    stack->pop(2);
    if(a == b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
//Compares two double-length numbers at the top of the stack for equality
int Dequals() {
    int64_t a = *(int64_t*)stack->at(1), b = *(int64_t*)stack->at(3);
    stack->pop(4);
    if(a == b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( n1 n2 -- f )
int lessThan(){
    int a = *(int*)stack->at(1), b = *(int*)stack->at(0);
    stack->pop(2);
    if(a < b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( u1 u2 -- f )
int UlessThan(){
    uint a = *stack->at(1), b = *stack->at(0);
    stack->pop(2);
    if(a < b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
int DlessThan() {
    int64_t a = *(int64_t*)stack->at(1), b = *(int64_t*)stack->at(3);
    stack->pop(4);
    if(a < b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( d1 d2 -- f )
int DUlessThan() {
    uint64_t a = *(uint64_t*)stack->at(1), b = *(uint64_t*)stack->at(3);
    stack->pop(4);
    if(a < b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( n1 n2 -- f )
int greaterThan(){
    int a = *(int*)stack->at(1), b = *(int*)stack->at(0);
    stack->pop(2);
    if(a > b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is equal to zero
int zeroEquals(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    if(a == 0)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( d -- f )
// Tests whether the double-length top of the stack is equal to zero
int DzeroEquals(){
    int64_t a = *(int64_t*)stack->at(1);
    stack->pop(2);
    if(a == 0)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is less than zero
int zeroLessThan(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    if(0 < a)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( n -- f )
// Tests whether the top of the stack is greater than zero
int zeroGreaterThan(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    if(0 > a)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}

// ( n addr -- )
// Stores n at the memory location pointed to by addr
int store() {
    int* ptr = (int*)*stack->at(0);
    int n = *(int*)stack->at(1);
    stack->pop(2);
    *ptr = n;
    return 0;
}

// ( d addr -- )
// Stores d at the memory location pointed to by addr
int store2() {
    int64_t* ptr = (int64_t*)*stack->at(0);
    int64_t n = *(int64_t*)stack->at(2);
    stack->pop(3);
    *ptr = n;
    return 0;
}

// ( c addr -- )
// Stores c at the memory location pointed to by addr
int c_store() {
    char *ptr = (char *) *stack->at(0);
    char n = *(char *) stack->at(1);
    stack->pop(2);
    *ptr = n;
    return 0;
}

// ( n addr -- )
// Adds n to the number stored at addr
int plus_store() {
    int* ptr = (int*)*stack->at(0);
    int n = *(int*)stack->at(1);
    stack->pop(2);
    *ptr += n;
    return 0;
}

// ( addr -- n )
// Pushes the contents of addr to the top of the stack
int fetch() {
    int* ptr = (int*)*stack->at(0);
    stack->pop(1);
    stack->push(*ptr);
    return 0;
}

// ( addr -- d )
// Pushes the double-length contents of addr to the top of the stack
int fetch2() {
    int64_t* ptr = (int64_t*)*stack->at(0);
    stack->pop(1);
    stack->push(*ptr);
    return 0;
}

// ( addr -- c )
// Pushes the byte pointed to by addr to the top of the stack
int c_fetch() {
    char* ptr = (char*)*stack->at(0);
    stack->pop(1);
    stack->push((int)*ptr);
    return 0;
}

// ( addr  -- n )
// Equivalent to @ .
int query() {
    fetch();
    print();
    return 0;
}

// ( n1 -- n2 )
// Multiplies n1 by 4
int cells() {
    *stack->at(0) *= 4;
    return 0;
}

// ( n c addr -- )
// Fills n1 bytes at addr with byte c
int fill() {
    char b = *(char*)stack->at(0);
    int n = *(int*)stack->at(1);
    char* addr = (char*)*stack->at(2);
    stack->pop(3);
    for(int i = 0; i < n; i++)
        addr[i] = b;
    return 0;
}

// ( n addr -- )
// Fills n bytes at addr with 0
int erase() {
    stack->push(0);
    return fill();
}

// ( u addr -- )
// Prints the contents of u bytes at addr
int dump() {
    uint c = *stack->at(0);
    int* addr = (int*)stack->at(1);
    stack->pop(2);
    for(uint i = 0; i < c; i++)
        if(i % 4 == 0) printf("%d ", addr[i / 4]);
    return 0;
}

// ( -- addr )
// Pushes the exectuion address of the next word in the input stream onto the stack
int tick() {
    char *tmp_buf, *tmp_idx;
    size_t i;
    GetSubstring(isspace(*tmp_idx));
    std::string str(tmp_buf);
    Function* func = find(str);
    stack->push((int) func);
    return 0;
}

// ( addr -- )
// Executes the word pointed to by addr
int execute() {
    Function* func = (Function*)*stack->at(0);
    stack->pop(1);
    run(func);
    return 0;
}

// ( -- )
//Clears the screen
int page() {
    system("clear");
    QUIT = true; //Avoid printing 'ok' after clearing the terminal, because it looks weird
    PAGE = true; //Avoid printing a double newline as well, because it too looks weird
    return 0;
}

// ( -- )
//Aborts the program
int abort_() {
    ABORT = true;
    stack->clear();
    throw 1;
}

// ( -- )
//Sets up the interpreter to not print ok
int quit() {
    QUIT = true;
    return 0;
}

// ( -- f )
// Pushes true if the stack is empty, pushes false otherwise
int stack_q() {
    if(!stack->size()) stack->push((int)0xffffffff);
    else stack->push(0x00000000);
    return 0;
}

// --------------------------------------------------------------
// STRUCTURAL/COMPILE-ONLY WORDS START HERE
// --------------------------------------------------------------

//Manages branching for if statements
//Branches to 0 if false, or to 1 if true.
int cond() {
    int a = *stack->at(0) != 0;
    stack->pop(1);
    return a;
}

//Initializes definite loops
int do_() {
    int index = *stack->at(0);
    int limit = *stack->at(1);
    stack->pop(2);
    return_stack->push(limit);
    return_stack->push(index);
    return 0;
}

//Definite loop conditional
int loop() {
    int index = *(int*)return_stack->at(0);
    int limit = *(int*)return_stack->at(1);
    index++;
    if(index != limit) {
        *(int*)return_stack->at(0) = index;
        return 1;
    } else {
        return_stack->pop(2);
        return 0;
    }
}

//Why does FORTH have to be inconsistent with its loop end condition, anyways?
int loop_plus() {
    int index = *(int*)return_stack->at(0);
    int limit = *(int*)return_stack->at(1);
    int inc = *(int*)stack->at(0);
    index += inc;
    if(inc < 0) {
        if(index >= limit) {
            *(int*)return_stack->at(0) = index;
            return 1;
        } else {
            return_stack->pop(2);
            return 0;
        }
    } else {
        if(index < limit) {
            *(int*)return_stack->at(0) = index;
            return 1;
        } else {
            return_stack->pop(2);
            return 0;
        }
    }
}

//Null operand for structural nodes
int nop() {
    return 0; //That's right; it does nothing.
}

//Nop-esque operand to handle loop break pathing
int leave() {
    return 1;
}

//Pushes a number onto the stack
void number(std::string& str) {
    if(!is_num(str)) {
        printf("%s ?", str.c_str());
        abort_();
        return;
    }
    bool db = false;
    for(size_t i = 0; i < str.size(); i++)
        if(str[i] == ',') {
            db = true;
            str.erase(i, 1);
        }

    if(db) {
        int64_t n = atol(str.c_str());
        stack->push(n);
    } else {
        int n = atoi(str.c_str());
        stack->push(n);
    }
}

//The terminal/file text interpreter
//Parses words (like :, VARIABLE) that can't be put into definitions
void text_interpreter() {
    while(*idx != '\0') {
        while (isspace(*idx)) idx++;
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
            MakeVar(Var((int) malloc(4), 4));
        } else if(str == "2VARIABLE") {
            MakeVar(Var((int) malloc(8), 8));
        } else if(str == "CONSTANT") {
            MakeVar(Var(*(int *) stack->at(0), 4));
            stack->pop(1);
        } else if(str == "2CONSTANT") {
            MakeVar(DoubleConst(*(int64_t*)stack->at(1)));
            stack->pop(2);
        } else if(str == "CREATE") {
            GetSubstring(isspace(*tmp_idx));
            glossary.push_back(std::make_pair(tmp_buf, new Var(0, 0)));

        // Allotment words
        } else if(str == "ALLOT") {
            Var *v = dynamic_cast<Var *>(glossary.back().second);
            if(v) {
                uint size = *stack->at(0);
                stack->pop(1);
                v->n = (int) realloc((void *) v->n, v->s + size);
                v->s += size;
            }
        } else if(str == ",") {
            Var *v = dynamic_cast<Var *>(glossary.back().second);
            if (v) {
                int n = *(int *) stack->at(0);
                stack->pop(1);
                v->n = (int) realloc((void *) v->n, v->s + 4);
                int *pt = (int *) (v->n + v->s);
                *pt = n;
                v->s += 4;
            }
        } else if(str == "C,") {
            Var *v = dynamic_cast<Var *>(glossary.back().second);
            if(v) {
                char c = *(char*) stack->at(0);
                stack->pop(1);
                v->n = (int) realloc((void *) v->n, v->s + 1);
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

            char* buf = (char*) malloc(n + 1);
            fread(buf, n, 1, curr_file);
            buf[n] = '\0';
            //Shuffle *idx around to make the text interpreter work
            char* idx_ = idx;
            idx = buf;
            text_interpreter();
            free(buf);
            idx = idx_;

            free(tmp_buf);
            fclose(curr_file);
        } else if (str == ".\"") {
                GetSubstring(*tmp_idx == '"');
                printf(tmp_buf);
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

int main() {
    //Initialize the stack & return stack
    stack = new Stack(4096);
    return_stack = new Stack(4096);

    //Build the glossary
    glossary.push_back(std::make_pair("CR", new Function(cr)));
    glossary.push_back(std::make_pair("SPACES", new Function(spaces)));
    glossary.push_back(std::make_pair("SPACE", new Function(space)));
    glossary.push_back(std::make_pair("EMIT", new Function(emit)));
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
    glossary.push_back(std::make_pair(">R", new Function(retPush)));
    glossary.push_back(std::make_pair("R>", new Function(retPop)));
    glossary.push_back(std::make_pair("I", new Function(retCopy)));
    glossary.push_back(std::make_pair("R@", new Function(retCopy)));
    glossary.push_back(std::make_pair("J", new Function(retCopy3)));
    glossary.push_back(std::make_pair(".", new Function(print)));
    glossary.push_back(std::make_pair("U.", new Function(uprint)));
    glossary.push_back(std::make_pair("D.", new Function(dprint)));
    glossary.push_back(std::make_pair("U.R", new Function(urjprint)));
    glossary.push_back(std::make_pair("D.R", new Function(drjprint)));
    glossary.push_back(std::make_pair(".S", new Function(printS)));
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
    glossary.push_back(std::make_pair("CELLS", new Function(cells)));
    glossary.push_back(std::make_pair("FILL", new Function(fill)));
    glossary.push_back(std::make_pair("ERASE", new Function(erase)));
    glossary.push_back(std::make_pair("DUMP", new Function(dump)));
    glossary.push_back(std::make_pair("'", new Function(tick)));
    glossary.push_back(std::make_pair("EXECUTE", new Function(execute)));
    glossary.push_back(std::make_pair("PAGE", new Function(page)));
    glossary.push_back(std::make_pair("QUIT", new Function(quit)));
    glossary.push_back(std::make_pair("?STACK", new Function(stack_q)));

    //Loop until terminal exit command is issued
    while(!BYE) {
        printf("#F> ");
        ReadInput(stdin);
        //Try and do something, and catch the abort if it throws
        try {
            text_interpreter();
        } catch(int) {} //Abort catching

        free(buf);
        //Newline printing and flag resetting
        if(S_UND) printf("stack underflow");
        if(!BYE && !ABORT && !QUIT && !S_UND) printf("ok");
        if(!BYE && !PAGE) printf("\n\n");
        if(QUIT) QUIT = false;
        if(ABORT) ABORT = false;
        if(S_UND) S_UND = false;
        if(PAGE) PAGE = false;
    }

    //Clean up the environment (no, not that kind, the other kind)
    delete stack;
    delete return_stack;
    for(auto itr = glossary.begin(); itr != glossary.end(); itr++)
        delete itr->second;

    return 0;
}

//UNSUPPORTED FEATURES:

//M*/ is not supported due to C/C++'s lack of support for 128 bit integers for 32 bit applications
