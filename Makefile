all:
	g++ -g -m32 -std=c++11 main.cpp Stack.cpp Function.cpp -o Mx_Forth.exe -Wall -Werror -Wextra -pedantic
