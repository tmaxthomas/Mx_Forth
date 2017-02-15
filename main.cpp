#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <stack>

#include "Stack.h"
#include "Function.h"

std::vector<std::pair<std::string, Function*> > glossary;

void addWord();
void forget();
void cr();
void spaces();
void space();
void emit();
void strPrint();
void add();
void sub();
void mult();
void div();
void mod();
void modDiv();
void swap();
void swap2();
void dup();
void dup2();
void over();
void over2();
void rot();
void drop();
void drop2();
void print();
void printS();
void comment();
void cond();
void number(std::string& str);
void parseFunc (std::vector<std::pair<std::string, Function*> >::reverse_iterator itr);


void addWord() {
    std::string name, func;
    std::cin >> name;
    Function* head, tail;
    std::cin >> func;
    while(func != ";") {
        //Comment handler
        if(func == "(") {
            while(func.at(func.size() - 1) != ')')
                std::cin >> func;
            std::cin >> func;
        }
        vec.push_back(func);
        std::cin >> func;
    }
    glossary.push_back(std::make_pair(name, vec));
}

void forget() {
    std::string name;
    std::cin >> name;
    auto itr = --glossary.end();
    for(; itr != glossary.begin(); itr--) {
        if(itr->first == name)
            break;
    }
    if(itr->first == name)
        glossary.erase(itr, glossary.end());
}

void cr() {
    std::cout << "\n";
}

void spaces() {
    std::string str(*(int*)stack->at(0), ' ');
    std::cout << str;
    stack->pop(1);
}

void space() {
    std::cout << " ";
}

void emit() {
    char ch = *(int*)stack->at(0);
    std::cout << ch;
    stack->pop(1);
}

void strPrint() {
    char* str = new char[1000];
    std::cin.getline(str, 1000, '"');
    std::cout << str;
    delete str;
}

void add() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) += s;
}

void sub() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) -= s;
}

void mult() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) *= s;
}

void div() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) /= s;
}

void mod() {
    int s = *(int*)stack->at(0);
    stack->pop(1);
    *(int*)stack->at(0) %= s;
}

void modDiv() {
    int m, s = *(int*)stack->at(0);
    stack->pop(1);
    m = *(int*)stack->at(0) % s;
    *(int*)stack->at(0) /= s;
    stack->push(m);
}

void swap() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int b = *(int*)stack->at(0);
    stack->pop(1);
    stack->push(t);
    stack->push(b);
}

void swap2() {
    long t = *(long*)stack->at(1);
    stack->pop(2);
    long b = *(long*)stack->at(1);
    stack->pop(2);
    stack->push(t);
    stack->push(b);
}

void dup() {
    stack->push(*(int*)stack->at(0));
}

void dup2() {
    stack->push(*(long*)stack->at(1));
}

void over() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int d = *(int*)stack->at(0);
    stack->push(t);
    stack->push(d);
}

void over2() {
    long t = *(long*)stack->at(1);
    stack->pop(2);
    long d = *(long*)stack->at(1);
    stack->push(t);
    stack->push(d);
}

void rot() {
    int t = *(int*)stack->at(0);
    stack->pop(1);
    int m = *(int*)stack->at(0);
    stack->pop(1);
    int b = *(int*)stack->at(0);
    stack->pop(1);
    stack->push(m);
    stack->push(t);
    stack->push(b);
}

void drop() {
    stack->pop(1);
}

void drop2() {
    stack->pop(2);
}

void print() {
    std::cout << *(int*)stack->at(0);
    stack->pop(1);
}

void printS() {
    std::list<double> buf;
    for(int a = 0; a < stack->size(); a++) {
        std::cout << *(int*)stack->at(a) << " ";
    }
}

void comment() {
    std::string dump;
    std::cin >> dump;
    while(dump.at(dump.size() - 1) != ')')
        std::cin >> dump;
}

void number(std::string& str) {
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
}

void cond() {
    if(stack->at(0) > 0) {

    } else {

    }
}


//NOTE TO SELF: REWRITE
void parseFunc (std::vector<std::pair<std::string, std::vector<std::string> > >::reverse_iterator itr){
    for(auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++) {
        auto itr3 = glossary.find(*itr2);
        auto itr4 = glossary.rbegin();
        for(; itr4 != usr_glossary.rend(); itr4++) {
            if(itr4->first == *itr2)
                break;
        }
        if(itr3 != glossary.end()) {
            itr3->second();
        } else if(itr4 != usr_glossary.rend()) {
            parseFunc(itr4);
        } else {
            number(*itr2);
        }
    }
}

int main() {
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
        auto itr = glossary.rbegin();
        for(; itr != glossary.rend(); itr++) {
            if(itr->first == str)
                break;
        }
        if(itr != glossary.rend()) {
            itr->second->();
        } else {
            number(str);
        }
    }
    return 0;
}