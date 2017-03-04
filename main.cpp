#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <stack>

#include "Stack.h"
#include "Function.h"

extern Stack *stack, *return_stack;

std::vector<std::pair<std::string, Function*> > glossary;
//Oh boy, header
int addWord();
int forget();
int cr();
int spaces();
int space();
int emit();
int strPrint();
int add();
int sub();
int mult();
int div();
int mod();
int modDiv();
int swap();
int swap2();
int dup();
int dup2();
int over();
int over2();
int rot();
int drop();
int drop2();
int print();
int printS();
int equals();
int lessThan();
int greaterThan();
int zeroEquals();
int zeroLessThan();
int zeroGreaterThan();
int cond();
int nop();
int number(std::string& str);

//Finds words in the glossary
Function* find(std::string& name) {
    for(auto itr = glossary.rbegin(); itr != glossary.rend(); itr++) {
        if(itr->first == name)
            return itr->second;
    }
    return NULL;
}

//Adds user-defined words to the glossary
int addWord() {
    std::string name, func;
    std::cin >> name;
    Function *head = NULL, *tail = head, *if_head = NULL, *loop_head = NULL;
    std::cin >> func;
    while(func != ";") {
        //Comment handler
        if(func == "(") {
            while(func.at(func.size() - 1) != ')')
                std::cin >> func;
            std::cin >> func;
        } else if(func == "IF") {
            Function* if_ = new Function(cond);
            tail->next = new Function*(if_);
            (*(tail->next))->next = new Function*[2];
            (*(tail->next))->next[0] = new Function(nop);
            if_head = *tail->next;
            tail = (*(tail->next))->next[0];
        } else if(func == "ELSE") {
            if(!if_head){
                std::cerr << "ERROR: THEN statement without corresponding IF statement\n";
                return 0;
            }
            Function* then = new Function(nop);
            tail->next = new Function*(then);
            if_head->next = tail->next;
            tail = *tail->next;
        } else {
            Function* temp;
            Function* tmp_ptr = find(func);
            if(tmp_ptr)
                temp = new Function(tmp_ptr);
            else
                temp = new Number(func);
            if(!head) {
                head = temp;
                tail = head;
            } else {
                tail->next = new Function*(temp);
                tail = *tail->next;
            }
        }
        std::cin >> func;
    }
    glossary.push_back(std::make_pair(name, head));
    return 0;
}

//Removes words from the glossary
int forget() {
    std::string name;
    std::cin >> name;
    auto itr = --glossary.end();
    for(; itr != glossary.begin(); itr--) {
        if(itr->first == name)
            break;
    }
    if(itr->first == name)
        glossary.erase(itr, glossary.end());
    return 0;
}
//Carriage return
int cr() {
    std::cout << "\n";
    return 0;
}
//Prints some number of spaces
int spaces() {
    std::string str(*(int*)stack->at(0), ' ');
    std::cout << str;
    stack->pop(1);
    return 0;
}
//Prints a space
int space() {
    std::cout << " ";
    return 0;
}
//Prints a character
int emit() {
    char ch = *(int*)stack->at(0);
    std::cout << ch;
    stack->pop(1);
    return 0;
}
//Prints a string-NOTE: NEEDS REVISION
int strPrint() {
    char* str = new char[1000];
    std::cin.getline(str, 1000, '"');
    std::cout << str;
    delete str;
    return 0;
}
//Polish postfix addition
int add() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) += s;
    return 0;
}
//Polish postfix subtraction
int sub() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) -= s;
    return 0;
}
//Polist postfix multiplication
int mult() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) *= s;
    return 0;
}
//Polish postfix division
int div() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) /= s;
    return 0;
}
//Postfix modulo
int mod() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) %= s;
    return 0;
}
//Postfix modulo & division
int modDiv() {
    int m, s = *(int*)stack->at(0);
    stack->pop(1);
    m = *(int*)stack->at(0) % s;
    *(int*)stack->at(0) /= s;
    stack->push(m);
    return 0;
}
//Swaps top two elements of the stack
int swap() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int b = *(int*)stack->at(0);
    stack->pop(1);
    stack->push(t);
    stack->push(b);
    return 0;
}
//Swaps top two elements of the stack for the next two
int swap2() {
    long t = *(long*)stack->at(1);
    stack->pop(2);
    long b = *(long*)stack->at(1);
    stack->pop(2);
    stack->push(t);
    stack->push(b);
    return 0;
}
//Duplicates the top of the stack
int dup() {
    stack->push(*(int*)stack->at(0));
    return 0;
}
//Duplicates the top tow elements of the stack
int dup2() {
    stack->push(*(long*)stack->at(1));
    return 0;
}
//Pushes the second element of the stack onto the stack
int over() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int d = *(int*)stack->at(0);
    stack->push(t);
    stack->push(d);
    return 0;
}
//Pushes the third and fourth elements of the stack onto the stack
int over2() {
    long t = *(long*)stack->at(1);
    stack->pop(2);
    long d = *(long*)stack->at(1);
    stack->push(t);
    stack->push(d);
    return 0;
}
//Removes the third element of the stack and pushes it onto the stack
int rot() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int m = *(int*)stack->at(0);
    stack->pop(1);
    int b = *(int*)stack->at(0);
    stack->pop(1);
    stack->push(m);
    stack->push(t);
    stack->push(b);
    return 0;
}
//Pops the top of the stack
int drop() {
    stack->pop(1);
    return 0;
}
//Pops the top 2 elements of the stack
int drop2() {
    stack->pop(2);
    return 0;
}
//Prints and then pops the top of the stack
int print() {
    std::cout << *(int*)stack->at(0);
    stack->pop(1);
    return 0;
}
//Prints the contents of the stack
int printS() {
    for(int a = 0; a < stack->size(); a++) {
        std::cout << *(int*)stack->at(a) << " ";
    }
    return 0;
}
int equals(){
    int a = *(int*)stack->at(0), b = *(int*)stack->at(1);
    stack->pop(2);
    if(a == b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}
int lessThan(){
    int a = *(int*)stack->at(0), b = *(int*)stack->at(1);
    stack->pop(2);
    if(a < b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}
int greaterThan(){
    int a = *(int*)stack->at(0), b = *(int*)stack->at(1);
    stack->pop(2);
    if(a > b)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}
int zeroEquals(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    if(a == 0)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}
int zeroLessThan(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    if(0 < a)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}
int zeroGreaterThan(){
    int a = *(int*)stack->at(0);
    stack->pop(1);
    if(0 > a)
        stack->push((int)0xffffffff);
    else
        stack->push(0x00000000);
    return 0;
}
//Manages branching for if statements
int cond() {
    return *(int*)stack->at(0) != 0;
}
//Null operand for structural nodes
int nop() {
    return 0;
}
//Pushes a number onto the stack
int number(std::string& str) {
    if(str.size() == 1) {
        if(str.at(0) >= '0' && str.at(0) <= '9')
            stack->push((int)std::stod(str));
        else {
            stack->push(str.at(0));
        }
    } else {
        size_t a;
        double v;
        v = stod(str, &a);
        if(a < str.size()) {
            std::cout << str << " ?";
            exit(1);
        } else {
            stack->push((int)v);
        }
    }
    return 0;
}

int main() {
    //Initializing stack
    stack = new Stack(4096);
    return_stack = new Stack(4096);
    //Generating FORTH environment
    glossary.push_back(std::make_pair(":", new Function(addWord)));
    glossary.push_back(std::make_pair("FORGET", new Function(forget)));
    glossary.push_back(std::make_pair("CR", new Function(cr)));
    glossary.push_back(std::make_pair("SPACES", new Function(spaces)));
    glossary.push_back(std::make_pair("SPACE", new Function(space)));
    glossary.push_back(std::make_pair("EMIT", new Function(emit)));
    glossary.push_back(std::make_pair(".\"", new Function(strPrint)));
    glossary.push_back(std::make_pair("+", new Function(add)));
    glossary.push_back(std::make_pair("-", new Function(sub)));
    glossary.push_back(std::make_pair("*", new Function(mult)));
    glossary.push_back(std::make_pair("/", new Function(div)));
    glossary.push_back(std::make_pair("MOD", new Function(mod)));
    glossary.push_back(std::make_pair("/MOD", new Function(modDiv)));
    glossary.push_back(std::make_pair("SWAP", new Function(swap)));
    glossary.push_back(std::make_pair("2SWAP", new Function(swap2)));
    glossary.push_back(std::make_pair("DUP", new Function(dup)));
    glossary.push_back(std::make_pair("2DUP", new Function(dup2)));
    glossary.push_back(std::make_pair("OVER", new Function(over)));
    glossary.push_back(std::make_pair("2OVER", new Function(over2)));
    glossary.push_back(std::make_pair("ROT", new Function(rot)));
    glossary.push_back(std::make_pair("DROP", new Function(drop)));
    glossary.push_back(std::make_pair("DROP2", new Function(drop2)));
    glossary.push_back(std::make_pair(".", new Function(print)));
    glossary.push_back(std::make_pair(".S", new Function(printS)));
    glossary.push_back(std::make_pair("=", new Function(equals)));
    glossary.push_back(std::make_pair("<", new Function(lessThan)));
    glossary.push_back(std::make_pair(">", new Function(greaterThan)));
    glossary.push_back(std::make_pair("0=", new Function(zeroEquals)));
    glossary.push_back(std::make_pair("0<", new Function(zeroLessThan)));
    glossary.push_back(std::make_pair("0>", new Function(zeroGreaterThan)));

    std::string str;
    while(std::cin) {
        std::cin >> str;
        Function* func = find(str);
        //if the input is a valid word
        if(func) {
            func->run();
        } else {
            number(str);
        }
    }
    //Destruction
    delete stack;
    delete return_stack;
    for(auto itr = glossary.begin(); itr != glossary.end(); itr++)
        delete itr->second;
    return 0;
}