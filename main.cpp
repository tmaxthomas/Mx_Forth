#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <stack>

#include "Stack.h"

Stack stack(4096);

std::map<std::string, void(*)() > glossary;
std::map<std::string, std::vector<std::string> > usr_glossary;

void addWord();
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
void dup();
void over();
void rot();
void drop();
void print();
void printS();
void number(std::string& str);
void parseFunc(std::map<std::string, std::vector<std::string> >::iterator);


void addWord() {
    std::string name, func;
    std::cin >> name;
    std::vector<std::string> vec;
    std::cin >> func;
    while(func != ";") {
        vec.push_back(func);
        std::cin >> func;
    }
    usr_glossary.insert(std::make_pair(name, vec));
}

void cr() {
    std::cout << "\n";
}

void spaces() {
    std::string str(*(int*)stack.at(0), ' ');
    std::cout << str;
    stack.pop();
}

void space() {
    std::cout << " ";
}

void emit() {
    char ch = *(int*)stack.at(0);
    std::cout << ch;
    stack.pop();
}

void strPrint() {
    char* str = new char[1000];
    std::cin.getline(str, 1000, '"');
    std::cout << str;
    delete str;
}

void add() {
    int s = *(int*)stack.at(0);
    stack.pop();
    *(int*)stack.at(0) += s;
}

void sub() {
    int s = *(int*)stack.at(0);
    stack.pop();
    *(int*)stack.at(0) -= s;
}

void mult() {
    int s = *(int*)stack.at(0);
    stack.pop();
    *(int*)stack.at(0) *= s;
}

void div() {
    int s = *(int*)stack.at(0);
    stack.pop();
    *(int*)stack.at(0) /= s;
}

void mod() {
    int s = *(int*)stack.at(0);
    stack.pop();
    *(int*)stack.at(0) %= s;
}

void modDiv() {
    int m, s = *(int*)stack.at(0);
    stack.pop();
    m = *(int*)stack.at(0) % s;
    *(int*)stack.at(0) /= s;
    stack.push(m);
}

void swap() {
    int t = *(int*)stack.at(0);
    stack.pop();
    int b = *(int*)stack.at(0);
    stack.pop();
    stack.push(t);
    stack.push(b);
}

void dup() {
    stack.push(*(int*)stack.at(0));
}

void over() {
    int t = *(int*)stack.at(0);
    stack.pop();
    int d = *(int*)stack.at(0);
    stack.push(t);
    stack.push(d);
}

void rot() {
    int t = *(int*)stack.at(0);
    stack.pop();
    int m = *(int*)stack.at(0);
    stack.pop();
    int b = *(int*)stack.at(0);
    stack.pop();
    stack.push(m);
    stack.push(t);
    stack.push(b);
}

void drop() {
    stack.pop();
}

void print() {
    std::cout << *(int*)stack.at(0);
    stack.pop();
}

void printS() {
    std::list<double> buf;
    for(int a = 0; a < stack.size(); a++) {
        std::cout << *(int*)stack.at(a) << " ";
    }
}

void number(std::string& str) {
    if(str.size() == 1) {
        if(str.at(0) >= '0' && str.at(0) <= '9')
            stack.push(std::stod(str));
        else {
            stack.push(str.at(0));
        }
    } else {
        size_t a;
        double v;
        v = stod(str, &a);
        if(a < str.size()) {
            std::cout << str << " ?";
            exit(1);
        } else {
            stack.push(v);
        }
    }
}

void parseFunc (std::map<std::string, std::vector<std::string> >::iterator itr){
    for(auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++) {
        auto itr3 = glossary.find(*itr2);
        auto itr4 = usr_glossary.find(*itr2);
        if(itr3 != glossary.end()) {
            itr3->second();
        } else if(itr4 != usr_glossary.end()) {
            parseFunc(itr4);
        } else {
            number(*itr2);
        }
    }
};

int main() {
    //Generating FORTH environment
    glossary[":"] = addWord;
    glossary["CR"] = cr;
    glossary["SPACES"] = spaces;
    glossary["SPACE"] = space;
    glossary["EMIT"] = emit;
    glossary[".\""] = strPrint;
    glossary["+"] = add;
    glossary["-"] = sub;
    glossary["*"] = mult;
    glossary["/"] = div;
    glossary["MOD"] = mod;
    glossary["/MOD"] = modDiv;
    glossary["SWAP"] = swap;
    glossary["DUP"] = dup;
    glossary["OVER"] = over;
    glossary["ROT"] = rot;
    glossary["DROP"] = drop;
    glossary[".S"] = printS;
    glossary["."] = print;

    std::string str;
    while(std::cin) {
        std::cin >> str;
        auto itr = glossary.find(str);
        auto itr2 = usr_glossary.find(str);
        if(itr != glossary.end()) {
            itr->second();
        } else if(itr2 != usr_glossary.end()) {
            parseFunc(itr2);
        } else {
            number(str);
        }
    }
    return 0;
}