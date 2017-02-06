#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <stack>

#include "Stack.h"

Stack stack(4096);

std::map<std::string, void(*)() > glossary;
std::vector<std::pair<std::string, std::vector<std::string> > > usr_glossary;

void wordParse(std::string& str);
void phraseParse(std::vector<std::string>&);

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

void wordParse(std::string& str) {
    auto itr = glossary.find(str);
    auto itr2 = usr_glossary.rbegin();
    for(; itr2 != usr_glossary.rend(); itr2++) {
        if(itr2->first == str)
            break;
    }
    if(itr != glossary.end()) {
        itr->second();
    } else if(itr2 != usr_glossary.rend()) {
        phraseParse(itr2->second);
    } else {
        number(str);
    }
}

void phraseParse(std::vector<std::string>& str_v) {
    for(auto vitr = str_v.begin(); vitr != str_v.end(); vitr++) {
        wordParse(*vitr);
    }
}

void addWord() {
    std::string name, func;
    std::cin >> name;
    std::vector<std::string> vec;
    std::cin >> func;
    while(func != ";") {
        if(func == "(") {
            while(func.at(func.size() - 1) != ')')
                std::cin >> func;
            std::cin >> func;
        }
        vec.push_back(func);
        std::cin >> func;
    }
    usr_glossary.push_back(std::make_pair(name, vec));
}

void forget() {
    std::string name;
    std::cin >> name;
    auto itr = --usr_glossary.end();
    for(; itr != usr_glossary.begin(); itr--) {
        if(itr->first == name)
            break;
    }
    if(itr->first == name)
        usr_glossary.erase(itr, usr_glossary.end());
}

void cr() {
    std::cout << "\n";
}

void spaces() {
    std::string str(*(int*)stack.at(0), ' ');
    std::cout << str;
    stack.pop(1);
}

void space() {
    std::cout << " ";
}

void emit() {
    char ch = *(int*)stack.at(0);
    std::cout << ch;
    stack.pop(1);
}

void strPrint() {
    char* str = new char[1000];
    std::cin.getline(str, 1000, '"');
    std::cout << str;
    delete str;
}

void add() {
    int s = *(int*)stack.at(0);
    stack.pop(1);
    *(int*)stack.at(0) += s;
}

void sub() {
    int s = *(int*)stack.at(0);
    stack.pop(1);
    *(int*)stack.at(0) -= s;
}

void mult() {
    int s = *(int*)stack.at(0);
    stack.pop(1);
    *(int*)stack.at(0) *= s;
}

void div() {
    int s = *(int*)stack.at(0);
    stack.pop(1);
    *(int*)stack.at(0) /= s;
}

void mod() {
    int s = *(int*)stack.at(0);
    stack.pop(1);
    *(int*)stack.at(0) %= s;
}

void modDiv() {
    int m, s = *(int*)stack.at(0);
    stack.pop(1);
    m = *(int*)stack.at(0) % s;
    *(int*)stack.at(0) /= s;
    stack.push(m);
}

void swap() {
    int t = *(int*)stack.at(0);
    stack.pop(1);
    int b = *(int*)stack.at(0);
    stack.pop(1);
    stack.push(t);
    stack.push(b);
}

void swap2() {
    long t = *(long*)stack.at(1);
    stack.pop(2);
    long b = *(long*)stack.at(1);
    stack.pop(2);
    stack.push(t);
    stack.push(b);
}

void dup() {
    stack.push(*(int*)stack.at(0));
}

void dup2() {
    stack.push(*(long*)stack.at(1));
}

void over() {
    int t = *(int*)stack.at(0);
    stack.pop(1);
    int d = *(int*)stack.at(0);
    stack.push(t);
    stack.push(d);
}

void over2() {
    long t = *(long*)stack.at(1);
    stack.pop(2);
    long d = *(long*)stack.at(1);
    stack.push(t);
    stack.push(d);
}

void rot() {
    int t = *(int*)stack.at(0);
    stack.pop(1);
    int m = *(int*)stack.at(0);
    stack.pop(1);
    int b = *(int*)stack.at(0);
    stack.pop(1);
    stack.push(m);
    stack.push(t);
    stack.push(b);
}

void drop() {
    stack.pop(1);
}

void drop2() {
    stack.pop(2);
}

void print() {
    std::cout << *(int*)stack.at(0);
    stack.pop(1);
}

void printS() {
    std::list<double> buf;
    for(int a = 0; a < stack.size(); a++) {
        std::cout << *(int*)stack.at(a) << " ";
    }
}

void comment() {
    std::string dump;
    std::cin >> dump;
    while(dump.at(dump.size() - 1) != ')')
        std::cin >> dump;
}

void cond(std::vector<std::string>& buf) {
    std::string buf;
    if(stack.at(0) > 0) {
        std::cin >> buf;
        while(buf != "THEN" && buf != "ELSE") {
            wordParse(buf);
            std::cin >> buf;
        }
        if(buf == "ELSE") {

        }
    } else {
        std::cin >> buf;
        while(buf != "THEN" && buf != "ELSE")
            std::cin >> buf;
        if(buf == "ELSE") {
            std::cin >> buf;
            while(buf != "THEN"){
                wordParse(buf);
                std::cin >> buf;
            }
        }
    }
}

void number(std::string& str) {
    if(str.size() == 1) {
        if(str.at(0) >= '0' && str.at(0) <= '9')
            stack.push((int)std::stod(str));
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
            stack.push((int)v);
        }
    }
}

int main() {
    //Generating FORTH environment
    glossary[":"] = addWord;
    glossary["FORGET"] = forget;
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
    glossary["2SWAP"] = swap2;
    glossary["DUP"] = dup;
    glossary["2DUP"] = dup2;
    glossary["OVER"] = over;
    glossary["2OVER"] = over2;
    glossary["ROT"] = rot;
    glossary["DROP"] = drop;
    glossary["2DROP"] = drop2;
    glossary["."] = print;
    glossary[".S"] = printS;
    glossary["("] = comment;

    std::string str;
    while(std::cin) {
        std::cin >> str;
        wordParse(str);
    }
    return 0;
}