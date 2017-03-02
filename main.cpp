#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <stack>

#include "Stack.h"
#include "Function.h"

extern Stack *stack, *return_stack;

std::vector<std::pair<std::string, Function*> > glossary;

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
int number(std::string& str);

Function* find(std::string& name) {
    for(auto itr = glossary.rbegin(); itr != glossary.rend(); itr++) {
        if(itr->first == name)
            return itr->second;
    }
    return NULL;
}

int addWord() {
    std::string name, func;
    std::cin >> name;
    Function *head = NULL, *tail = head;
    std::cin >> func;
    while(func != ";") {
        //Comment handler
        if(func == "(") {
            while(func.at(func.size() - 1) != ')')
                std::cin >> func;
            std::cin >> func;
        }
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
        std::cin >> func;
    }
    glossary.push_back(std::make_pair(name, head));
    return 0;
}

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

int cr() {
    std::cout << "\n";
    return 0;
}

int spaces() {
    std::string str(*(int*)stack->at(0), ' ');
    std::cout << str;
    stack->pop(1);
    return 0;
}

int space() {
    std::cout << " ";
    return 0;
}

int emit() {
    char ch = *(int*)stack->at(0);
    std::cout << ch;
    stack->pop(1);
    return 0;
}

int strPrint() {
    char* str = new char[1000];
    std::cin.getline(str, 1000, '"');
    std::cout << str;
    delete str;
    return 0;
}

int add() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) += s;
    return 0;
}

int sub() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) -= s;
    return 0;
}

int mult() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) *= s;
    return 0;
}

int div() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) /= s;
    return 0;
}

int mod() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) %= s;
    return 0;
}

int modDiv() {
    int m, s = *(int*)stack->at(0);
    stack->pop(1);
    m = *(int*)stack->at(0) % s;
    *(int*)stack->at(0) /= s;
    stack->push(m);
    return 0;
}

int swap() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int b = *(int*)stack->at(0);
    stack->pop(1);
    stack->push(t);
    stack->push(b);
    return 0;
}

int swap2() {
    long t = *(long*)stack->at(1);
    stack->pop(2);
    long b = *(long*)stack->at(1);
    stack->pop(2);
    stack->push(t);
    stack->push(b);
    return 0;
}

int dup() {
    stack->push(*(int*)stack->at(0));
    return 0;
}

int dup2() {
    stack->push(*(long*)stack->at(1));
    return 0;
}

int over() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int d = *(int*)stack->at(0);
    stack->push(t);
    stack->push(d);
    return 0;
}

int over2() {
    long t = *(long*)stack->at(1);
    stack->pop(2);
    long d = *(long*)stack->at(1);
    stack->push(t);
    stack->push(d);
    return 0;
}

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

int drop() {
    stack->pop(1);
    return 0;
}

int drop2() {
    stack->pop(2);
    return 0;
}

int print() {
    std::cout << *(int*)stack->at(0);
    stack->pop(1);
    return 0;
}

int printS() {
    for(int a = 0; a < stack->size(); a++) {
        std::cout << *(int*)stack->at(a) << " ";
    }
    return 0;
}

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

    std::string str;
    while(std::cin) {
        std::cin >> str;
        Function* func = find(str);
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