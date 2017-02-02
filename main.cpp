#include <iostream>
#include <map>
#include <vector>
#include <stack>

std::map<std::string, std::vector<Function> > glossary;
std::stack<double> stack;

class Function {
public:
    Function(void(*fxn)(), std::string str = "") : val(0) {
        if(str.size() > 0) {
            if(str.size() == 1) {
                if(str.at(0) >= '0' && str.at(0) <= '9')
                    val = std::stod(str);
                else {
                    val = str.at(0);
                }
            } else {
                size_t a;
                double v;
                v = stod(str, &a);
                if(a < str.size()) {
                    std::cout << str << " ?";
                    exit(1);
                } else {
                    val = v;
                }
            }
        }
    }
    void operator() () {
        if(val > 0)
            number();
        else
            func();
    }
private:
    void number() {
        stack.push(val);
    }
    double val;
    void(*func)();
};

void addWord();
void cr();
void spaces();
void space();
void emit();
void strPrint();
void add();
void print();


void addWord() {
    std::string name, func;
    std::cin >> name;
    std::vector<Function> vec;
    std::cin >> func;
    while(func != ";") {
        std::map<std::string, std::vector<Function> >::iterator itr = glossary.find(func);
        if(itr != glossary.end())
            vec.insert(vec.end(), itr->second.begin(), itr->second.end());
        else {
            vec.push_back(Function(NULL, func));
        }
    }
}

void cr() {
    std::cout << "\n";
}

void spaces() {
    std::string str(stack.top(), ' ');
    std::cout << str;
    stack.pop();
}

void space() {
    std::cout << " ";
}

void emit() {
    char ch = stack.top();
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
    int s = stack.top();
    stack.pop();
    stack.top() += s;
}

void print() {
    std::cout << stack.top();
    stack.pop();
}

int main() {
    //Generating FORTH environment
    Function addWordf(addWord);
    glossary[":"].push_back(addWordf);
    Function crf(cr);
    glossary["CR"].push_back(crf);
    Function spacesf(spaces);
    glossary["SPACES"].push_back(spacesf);
    Function spacef(space);
    glossary["SPACE"].push_back(spacef);
    Function emitf(emit);
    glossary["EMIT"].push_back(emitf);
    Function strPrintf(strPrint);
    glossary[".\""].push_back(strPrintf);
    Function addf(add);
    glossary["+"].push_back(addf);
    Function printf(print);
    glossary["."].push_back(printf);

    std::string str;
    while(std::cin) {
        std::cin >> str;
        auto itr = glossary.find(str);
        if(itr != glossary.end()) {
            for(auto itr2 = itr->second.begin(); itr2 != itr->second.end(); itr2++) {
                itr2->();
            }
        } else {
            Function number(NULL, str);
            number();
        }
    }
    return 0;
}